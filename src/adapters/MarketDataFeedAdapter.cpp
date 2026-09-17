#include "adapters/MarketDataFeedAdapter.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <optional>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace risk_engine {

constexpr size_t MAX_MARKET_DATA_MSG_SIZE = 4096;
constexpr int RECONNECT_DELAY_MS = 1000;
constexpr int MAX_RECONNECT_ATTEMPTS = 10;
constexpr size_t BATCH_SIZE = 100;

MarketDataFeedAdapter::MarketDataFeedAdapter(
    std::shared_ptr<disruptor::RingBuffer<MarketDataEvent>> ring_buffer,
    const FeedConfig& config)
    : ring_buffer_(ring_buffer)
    , config_(config)
    , running_(false)
    , reconnect_attempts_(0)
    , last_sequence_(0)
    , messages_received_(0)
    , messages_dropped_(0) {
    
    SPDLOG_INFO("MarketDataFeedAdapter initializing with feed URL: {}", config.feed_url);
    setupLogging();
}

void MarketDataFeedAdapter::setupLogging() {
    try {
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/market_data_feed.log", 1024 * 1024 * 10, 3);
        logger_ = std::make_shared<spdlog::logger>("market_data_feed", rotating_sink);
        logger_->set_level(spdlog::level::info);
        logger_->set_pattern("%Y-%m-%d %H:%M:%S.%f [%l] %v");
    } catch (const spdlog::spdlog_ex& ex) {
        SPDLOG_WARN("Failed to create rotating logger: {}", ex.what());
        logger_ = spdlog::default_logger();
    }
}

bool MarketDataFeedAdapter::connect() {
    std::lock_guard<std::mutex> lock(connection_mutex_);
    
    if (socket_fd_ >= 0) {
        SPDLOG_INFO("Already connected to market data feed");
        return true;
    }

    SPDLOG_INFO("Connecting to market data feed: {}", config_.feed_url);
    
    std::string host;
    int port;
    if (!parseFeedUrl(config_.feed_url, host, port)) {
        SPDLOG_ERROR("Failed to parse feed URL: {}", config_.feed_url);
        return false;
    }

    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        SPDLOG_ERROR("Failed to create socket: {}", strerror(errno));
        return false;
    }

    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        SPDLOG_ERROR("Failed to resolve host: {}", host);
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        SPDLOG_ERROR("Failed to connect: {}", strerror(errno));
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    int flags = fcntl(socket_fd_, F_GETFL, 0);
    fcntl(socket_fd_, F_SETFL, flags | O_NONBLOCK);

    SPDLOG_INFO("Connected to market data feed successfully");
    reconnect_attempts_ = 0;
    return true;
}

bool MarketDataFeedAdapter::parseFeedUrl(const std::string& url, std::string& host, int& port) {
    size_t protocol_end = url.find("://");
    std::string stripped_url = (protocol_end != std::string::npos) 
        ? url.substr(protocol_end + 3) 
        : url;
    
    size_t colon_pos = stripped_url.rfind(':');
    if (colon_pos == std::string::npos) {
        host = stripped_url;
        port = config_.default_port;
        return true;
    }
    
    host = stripped_url.substr(0, colon_pos);
    try {
        port = std::stoi(stripped_url.substr(colon_pos + 1));
    } catch (...) {
        port = config_.default_port;
    }
    return true;
}

void MarketDataFeedAdapter::disconnect() {
    std::lock_guard<std::mutex> lock(connection_mutex_);
    
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
        SPDLOG_INFO("Disconnected from market data feed");
    }
}

void MarketDataFeedAdapter::start() {
    if (running_.exchange(true)) {
        SPDLOG_WARN("MarketDataFeedAdapter already running");
        return;
    }

    if (!connect()) {
        running_ = false;
        SPDLOG_ERROR("Failed to connect to market data feed");
        return;
    }

    worker_thread_ = std::thread([this]() {
        SPDLOG_INFO("MarketDataFeedAdapter worker thread started");
        runEventLoop();
        SPDLOG_INFO("MarketDataFeedAdapter worker thread stopped");
    });
}

void MarketDataFeedAdapter::stop() {
    if (!running_.exchange(false)) {
        SPDLOG_INFO("MarketDataFeedAdapter already stopped");
        return;
    }

    disconnect();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    SPDLOG_INFO("MarketDataFeedAdapter stopped. Total messages: {}, dropped: {}", 
                messages_received_.load(), messages_dropped_.load());
}

void MarketDataFeedAdapter::runEventLoop() {
    char buffer[MAX_MARKET_DATA_MSG_SIZE];
    std::vector<char> message_buffer;
    message_buffer.reserve(8192);

    while (running_.load()) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        
        {
            std::lock_guard<std::mutex> lock(connection_mutex_);
            if (socket_fd_ >= 0) {
                FD_SET(socket_fd_, &read_fds);
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(socket_fd_ + 1, &read_fds, nullptr, nullptr, &timeout);
        
        if (activity < 0) {
            if (running_.load()) {
                SPDLOG_ERROR("Select error: {}", strerror(errno));
                handleReconnect();
            }
            continue;
        }

        if (activity == 0) {
            continue;
        }

        ssize_t bytes_read = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read <= 0) {
            if (bytes_read < 0 && errno == EAGAIN) {
                continue;
            }
            SPDLOG_WARN("Connection closed or error, bytes_read: {}", bytes_read);
            handleReconnect();
            continue;
        }

        buffer[bytes_read] = '\0';
        message_buffer.insert(message_buffer.end(), buffer, buffer + bytes_read);
        
        processMessageBuffer(message_buffer);
    }
}

void MarketDataFeedAdapter::processMessageBuffer(std::vector<char>& buffer) {
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

        std::string message(buffer.begin(), buffer.begin() + newline_pos);
        buffer.erase(buffer.begin(), buffer.begin() + newline_pos + 2);

        auto event = parseMarketDataMessage(message);
        if (event) {
            publishEvent(*event);
        }
    }
}

std::optional<MarketDataEvent> MarketDataFeedAdapter::parseMarketDataMessage(
    const std::string& raw_message) {
    
    try {
        auto json = nlohmann::json::parse(raw_message);
        
        MarketDataEvent event;
        event.timestamp = std::chrono::system_clock::now();
        event.sequence = last_sequence_.fetch_add(1);
        
        if (json.contains("type")) {
            std::string type = json["type"];
            
            if (type == "trade") {
                event.event_type = MarketDataEventType::TRADE;
                event.instrument_id = json.value("instrument_id", "");
                event.price = json.value("price", 0.0);
                event.quantity = json.value("quantity", 0);
                event.side = json.value("side", "") == "buy" ? OrderSide::BUY : OrderSide::SELL;
            } else if (type == "quote") {
                event.event_type = MarketDataEventType::QUOTE;
                event.instrument_id = json.value("instrument_id", "");
                event.bid_price = json.value("bid", 0.0);
                event.ask_price = json.value("ask", 0.0);
                event.bid_size = json.value("bid_size", 0);
                event.ask_size = json.value("ask_size", 0);
            } else if (type == "orderbook") {
                event.event_type = MarketDataEventType::ORDERBOOK_SNAPSHOT;
                event.instrument_id = json.value("instrument_id", "");
                event.bid_price = json.value("best_bid", 0.0);
                event.ask_price = json.value("best_ask", 0.0);
            }
        }

        messages_received_.fetch_add(1);
        SPDLOG_TRACE("Parsed event: type={}, instrument={}, price={}", 
                     static_cast<int>(event.event_type), 
                     event.instrument_id, 
                     event.price);
        
        return event;
        
    } catch (const nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("JSON parse error: {}", e.what());
        messages_dropped_.fetch_add(1);
        return std::nullopt;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error parsing market data message: {}", e.what());
        messages_dropped_.fetch_add(1);
        return std::nullopt;
    }
}

void MarketDataFeedAdapter::publishEvent(const MarketDataEvent& event) {
    if (!ring_buffer_) {
        SPDLOG_ERROR("Ring buffer not initialized");
        return;
    }

    auto sequence = ring_buffer_->next();
    try {
        auto* slot = ring_buffer_->get(sequence);
        *slot = event;
        ring_buffer_->publish(sequence);
        
        SPDLOG_TRACE("Published event with sequence: {}", sequence);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to publish event: {}", e.what());
    }
}

void MarketDataFeedAdapter::handleReconnect() {
    disconnect();
    
    if (reconnect_attempts_.load() >= MAX_RECONNECT_ATTEMPTS) {
        SPDLOG_ERROR("Max reconnect attempts reached");
        running_ = false;
        return;
    }

    reconnect_attempts_.fetch_add(1);
    SPDLOG_INFO("Reconnecting to market data feed, attempt {}/{}", 
                reconnect_attempts_.load(), MAX_RECONNECT_ATTEMPTS);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_DELAY_MS));
    connect();
}

std::atomic<uint64_t> MarketDataFeedAdapter::getMessagesReceived() const {
    return messages_received_;
}

std::atomic<uint64_t> MarketDataFeedAdapter::getMessagesDropped() const {
    return messages_dropped_;
}

bool MarketDataFeedAdapter::isConnected() const {
    return socket_fd_ >= 0;
}

}