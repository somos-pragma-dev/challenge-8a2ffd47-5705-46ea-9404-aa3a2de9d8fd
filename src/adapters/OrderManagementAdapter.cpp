#include "adapters/OrderManagementAdapter.h"
#include "domain/Order.h"
#include "domain/RiskLimits.h"
#include "core/CircuitBreaker.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <unordered_map>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace risk_engine {

constexpr int DEFAULT_EXCHANGE_PORT = 8080;
constexpr int ORDER_TIMEOUT_MS = 5000;
constexpr int MAX_PENDING_ORDERS = 10000;
constexpr size_t EXCHANGE_MSG_BUFFER = 8192;

OrderManagementAdapter::OrderManagementAdapter(
    std::shared_ptr<disruptor::RingBuffer<OrderEvent>> order_ring_buffer,
    std::shared_ptr<CircuitBreakerManager> circuit_breaker_manager,
    const ExchangeConfig& config)
    : order_ring_buffer_(order_ring_buffer)
    , circuit_breaker_manager_(circuit_breaker_manager)
    , config_(config)
    , running_(false)
    , orders_sent_(0)
    , orders_accepted_(0)
    , orders_rejected_(0)
    , last_order_id_(0) {
    
    SPDLOG_INFO("OrderManagementAdapter initializing with exchange: {}", config.exchange_url);
    setupLogging();
    initializeCircuitBreakers();
}

void OrderManagementAdapter::setupLogging() {
    try {
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/order_management.log", 1024 * 1024 * 10, 5);
        logger_ = std::make_shared<spdlog::logger>("order_management", rotating_sink);
        logger_->set_level(spdlog::level::debug);
        logger_->set_pattern("%Y-%m-%d %H:%M:%S.%f [%l] [orderId=%i] %v");
    } catch (const spdlog::spdlog_ex& ex) {
        SPDLOG_WARN("Failed to create order management logger: {}", ex.what());
        logger_ = spdlog::default_logger();
    }
}

void OrderManagementAdapter::initializeCircuitBreakers() {
    if (!circuit_breaker_manager_) {
        SPDLOG_WARN("Circuit breaker manager not provided");
        return;
    }

    for (const auto& instrument : config_.instruments) {
        auto cb = circuit_breaker_manager_->getOrCreateCircuitBreaker(
            instrument, 
            config_.circuit_breaker_threshold,
            config_.circuit_breaker_timeout);
        
        SPDLOG_INFO("Circuit breaker initialized for instrument: {}", instrument);
    }
}

bool OrderManagementAdapter::connect() {
    std::lock_guard<std::mutex> lock(exchange_mutex_);
    
    if (exchange_socket_ >= 0) {
        SPDLOG_INFO("Already connected to exchange");
        return true;
    }

    SPDLOG_INFO("Connecting to exchange: {}", config_.exchange_url);
    
    std::string host;
    int port;
    if (!parseExchangeUrl(config_.exchange_url, host, port)) {
        SPDLOG_ERROR("Failed to parse exchange URL: {}", config_.exchange_url);
        return false;
    }

    exchange_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (exchange_socket_ < 0) {
        SPDLOG_ERROR("Failed to create exchange socket: {}", strerror(errno));
        return false;
    }

    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        SPDLOG_ERROR("Failed to resolve exchange host: {}", host);
        close(exchange_socket_);
        exchange_socket_ = -1;
        return false;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(exchange_socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(exchange_socket_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (::connect(exchange_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        SPDLOG_ERROR("Failed to connect to exchange: {}", strerror(errno));
        close(exchange_socket_);
        exchange_socket_ = -1;
        return false;
    }

    SPDLOG_INFO("Connected to exchange successfully");
    return true;
}

bool OrderManagementAdapter::parseExchangeUrl(const std::string& url, 
                                               std::string& host, int& port) {
    size_t protocol_end = url.find("://");
    std::string stripped_url = (protocol_end != std::string::npos) 
        ? url.substr(protocol_end + 3) 
        : url;
    
    size_t colon_pos = stripped_url.rfind(':');
    if (colon_pos == std::string::npos) {
        host = stripped_url;
        port = DEFAULT_EXCHANGE_PORT;
        return true;
    }
    
    host = stripped_url.substr(0, colon_pos);
    try {
        port = std::stoi(stripped_url.substr(colon_pos + 1));
    } catch (...) {
        port = DEFAULT_EXCHANGE_PORT;
    }
    return true;
}

void OrderManagementAdapter::disconnect() {
    std::lock_guard<std::mutex> lock(exchange_mutex_);
    
    if (exchange_socket_ >= 0) {
        close(exchange_socket_);
        exchange_socket_ = -1;
        SPDLOG_INFO("Disconnected from exchange");
    }
}

void OrderManagementAdapter::start() {
    if (running_.exchange(true)) {
        SPDLOG_WARN("OrderManagementAdapter already running");
        return;
    }

    if (!connect()) {
        running_ = false;
        SPDLOG_ERROR("Failed to connect to exchange");
        return;
    }

    response_handler_thread_ = std::thread([this]() {
        SPDLOG_INFO("OrderManagementAdapter response handler started");
        handleResponses();
        SPDLOG_INFO("OrderManagementAdapter response handler stopped");
    });

    SPDLOG_INFO("OrderManagementAdapter started");
}

void OrderManagementAdapter::stop() {
    if (!running_.exchange(false)) {
        SPDLOG_INFO("OrderManagementAdapter already stopped");
        return;
    }

    disconnect();
    
    if (response_handler_thread_.joinable()) {
        response_handler_thread_.join();
    }
    
    logStatistics();
}

void OrderManagementAdapter::handleResponses() {
    char buffer[EXCHANGE_MSG_BUFFER];
    std::vector<char> response_buffer;
    response_buffer.reserve(16384);

    while (running_.load()) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        
        {
            std::lock_guard<std::mutex> lock(exchange_mutex_);
            if (exchange_socket_ >= 0) {
                FD_SET(exchange_socket_, &read_fds);
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;

        int activity = select(exchange_socket_ + 1, &read_fds, nullptr, nullptr, &timeout);
        
        if (activity <= 0) {
            continue;
        }

        ssize_t bytes_read = recv(exchange_socket_, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read <= 0) {
            SPDLOG_WARN("Exchange connection closed");
            continue;
        }

        buffer[bytes_read] = '\0';
        response_buffer.insert(response_buffer.end(), buffer, buffer + bytes_read);
        
        processResponseBuffer(response_buffer);
    }
}

void OrderManagementAdapter::processResponseBuffer(std::vector<char>& buffer) {
    while (true) {
        size_t newline_pos = 0;
        for (size_t i = 0; i + 1 < buffer.size(); ++i) {
            if (buffer[i] == '\n' && buffer[i + 1] == '\n') {
                newline_pos = i;
                break;
            }
        }

        if (newline_pos == 0) {
            break;
        }

        std::string response(buffer.begin(), buffer.begin() + newline_pos);
        buffer.erase(buffer.begin(), buffer.begin() + newline_pos + 2);

        processExchangeResponse(response);
    }
}

void OrderManagementAdapter::processExchangeResponse(const std::string& response) {
    try {
        auto json = nlohmann::json::parse(response);
        
        std::string order_id = json.value("order_id", "");
        std::string status = json.value("status", "");
        std::string instrument_id = json.value("instrument_id", "");
        
        auto it = pending_orders_.find(order_id);
        if (it == pending_orders_.end()) {
            SPDLOG_WARN("Received response for unknown order: {}", order_id);
            return;
        }

        Order& order = it->second;
        
        if (status == "accepted" || status == "filled") {
            order.status = OrderStatus::FILLED;
            orders_accepted_.fetch_add(1);
            SPDLOG_INFO("Order accepted: {} instrument={} qty={} price={}",
                        order_id, instrument_id, order.quantity, order.price);
            
            publishOrderEvent(order, OrderEventType::ORDER_ACCEPTED);
        } else if (status == "rejected") {
            order.status = OrderStatus::REJECTED;
            orders_rejected_.fetch_add(1);
            std::string reason = json.value("reason", "unknown");
            SPDLOG_WARN("Order rejected: {} reason={}", order_id, reason);
            
            publishOrderEvent(order, OrderEventType::ORDER_REJECTED);
            
            if (circuit_breaker_manager_) {
                circuit_breaker_manager_->recordRejection(instrument_id);
            }
        }

        pending_orders_.erase(it);
        
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error processing exchange response: {}", e.what());
    }
}

OrderResult OrderManagementAdapter::submitOrder(const Order& order) {
    if (!running_.load()) {
        return {false, "Adapter not running", 0};
    }

    if (circuit_breaker_manager_) {
        auto cb = circuit_breaker_manager_->getCircuitBreaker(order.instrument_id);
        if (cb && cb->isOpen()) {
            SPDLOG_WARN("Circuit breaker open for instrument: {}", order.instrument_id);
            return {false, "Circuit breaker open", 0};
        }
    }

    if (pending_orders_.size() >= MAX_PENDING_ORDERS) {
        SPDLOG_ERROR("Too many pending orders");
        return {false, "Too many pending orders", 0};
    }

    std::string order_id = generateOrderId(order);
    Order order_with_id = order;
    order_with_id.order_id = order_id;
    order_with_id.submitted_at = std::chrono::system_clock::now();

    std::lock_guard<std::mutex> lock(exchange_mutex_);
    if (exchange_socket_ < 0) {
        return {false, "Not connected to exchange", 0};
    }

    auto json = serializeOrder(order_with_id);
    std::string message = json.dump() + "\n\n";
    
    ssize_t sent = send(exchange_socket_, message.c_str(), message.size(), 0);
    if (sent < 0) {
        SPDLOG_ERROR("Failed to send order: {}", strerror(errno));
        return {false, "Failed to send order", 0};
    }

    pending_orders_[order_id] = order_with_id;
    orders_sent_.fetch_add(1);
    
    SPDLOG_INFO("Order submitted: {} instrument={} qty={} price={}",
                order_id, order.instrument_id, order.quantity, order.price);
    
    publishOrderEvent(order_with_id, OrderEventType::ORDER_SUBMITTED);
    
    return {true, "", std::stoull(order_id)};
}

std::string OrderManagementAdapter::generateOrderId(const Order& order) {
    uint64_t id = last_order_id_.fetch_add(1);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    return std::to_string(timestamp) + "-" + std::to_string(id);
}

nlohmann::json OrderManagementAdapter::serializeOrder(const Order& order) {
    nlohmann::json json;
    json["order_id"] = order.order_id;
    json["instrument_id"] = order.instrument_id;
    json["side"] = order.side == OrderSide::BUY ? "buy" : "sell";
    json["type"] = order.order_type == OrderType::MARKET ? "market" : "limit";
    json["quantity"] = order.quantity;
    json["price"] = order.price;
    json["trader_id"] = order.trader_id;
    json["strategy_id"] = order.strategy_id;
    json["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        order.submitted_at.time_since_epoch()).count();
    return json;
}

void OrderManagementAdapter::publishOrderEvent(const Order& order, OrderEventType event_type) {
    if (!order_ring_buffer_) {
        SPDLOG_ERROR("Order ring buffer not initialized");
        return;
    }

    auto sequence = order_ring_buffer_->next();
    try {
        auto* slot = order_ring_buffer_->get(sequence);
        slot->order_id = order.order_id;
        slot->instrument_id = order.instrument_id;
        slot->event_type = event_type;
        slot->timestamp = std::chrono::system_clock::now();
        slot->quantity = order.quantity;
        slot->price = order.price;
        order_ring_buffer_->publish(sequence);
        
        SPDLOG_TRACE("Published order event: {} type={}", order.order_id, static_cast<int>(event_type));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to publish order event: {}", e.what());
    }
}

void OrderManagementAdapter::logStatistics() {
    SPDLOG_INFO("OrderManagementAdapter Statistics:");
    SPDLOG_INFO("  Orders sent: {}", orders_sent_.load());
    SPDLOG_INFO("  Orders accepted: {}", orders_accepted_.load());
    SPDLOG_INFO("  Orders rejected: {}", orders_rejected_.load());
    SPDLOG_INFO("  Pending orders: {}", pending_orders_.size());
}

std::atomic<uint64_t> OrderManagementAdapter::getOrdersSent() const {
    return orders_sent_;
}

std::atomic<uint64_t> OrderManagementAdapter::getOrdersAccepted() const {
    return orders_accepted_;
}

std::atomic<uint64_t> OrderManagementAdapter::getOrdersRejected() const {
    return orders_rejected_;
}

bool OrderManagementAdapter::isConnected() const {
    return exchange_socket_ >= 0;
}

}