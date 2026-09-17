#pragma once

#include <atomic>
#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

namespace risk_engine {
namespace domain {

using Timestamp = std::chrono::microseconds;
using Price = int64_t;
using Quantity = int64_t;
constexpr auto PRICE_MULTIPLIER = 1000000LL;

enum class Side : uint8_t { BUY = 0, SELL = 1 };

enum class MarketDataType : uint8_t {
    BID = 0,
    ASK = 1,
    TRADE = 2,
    SNAPSHOT = 3
};

struct alignas(64) PriceLevel {
    Price price_{0};
    Quantity quantity_{0};
    uint32_t orderCount_{0};
    std::atomic<uint64_t> lastUpdate_{0};

    PriceLevel() noexcept = default;

    void update(Price price, Quantity quantity, uint32_t orders) noexcept {
        price_ = price;
        quantity_ = quantity;
        orderCount_ = orders;
        lastUpdate_.store(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count(),
            std::memory_order_relaxed);
    }

    void clear() noexcept {
        price_ = 0;
        quantity_ = 0;
        orderCount_ = 0;
    }

    [[nodiscard]] bool isEmpty() const noexcept { return quantity_ == 0; }
};

template<size_t MaxLevels = 10>
struct alignas(64) OrderBookLevel2 {
    static constexpr size_t kMaxLevels = MaxLevels;

    std::array<PriceLevel, MaxLevels> bids_;
    std::array<PriceLevel, MaxLevels> asks_;

    std::atomic<uint64_t> sequence_{0};
    std::atomic<uint64_t> lastUpdateNs_{0};
    uint32_t instrumentId_{0};
    std::atomic<bool> isStale_{false};

    OrderBookLevel2() noexcept {
        bids_.fill(PriceLevel{});
        asks_.fill(PriceLevel{});
    }

    void updateBid(size_t level, Price price, Quantity quantity, uint32_t orders) noexcept {
        if (level < MaxLevels) {
            bids_[level].update(price, quantity, orders);
            updateTimestamp();
        }
    }

    void updateAsk(size_t level, Price price, Quantity quantity, uint32_t orders) noexcept {
        if (level < MaxLevels) {
            asks_[level].update(price, quantity, orders);
            updateTimestamp();
        }
    }

    void updateTimestamp() noexcept {
        lastUpdateNs_.store(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count(),
            std::memory_order_release);
    }

    [[nodiscard]] Price bestBid() const noexcept {
        for (const auto& level : bids_) {
            if (!level.isEmpty()) return level.price_;
        }
        return 0;
    }

    [[nodiscard]] Price bestAsk() const noexcept {
        for (const auto& level : asks_) {
            if (!level.isEmpty()) return level.price_;
        }
        return 0;
    }

    [[nodiscard]] Price midPrice() const noexcept {
        Price bid = bestBid();
        Price ask = bestAsk();
        if (bid > 0 && ask > 0) {
            return (bid + ask) / 2;
        }
        return 0;
    }

    [[nodiscard]] Price spread() const noexcept {
        Price ask = bestAsk();
        Price bid = bestBid();
        if (ask > 0 && bid > 0) {
            return ask - bid;
        }
        return 0;
    }

    [[nodiscard]] Quantity bidLiquidity(size_t levels) const noexcept {
        Quantity total = 0;
        size_t count = std::min(levels, MaxLevels);
        for (size_t i = 0; i < count; ++i) {
            total += bids_[i].quantity_;
        }
        return total;
    }

    [[nodiscard]] Quantity askLiquidity(size_t levels) const noexcept {
        Quantity total = 0;
        size_t count = std::min(levels, MaxLevels);
        for (size_t i = 0; i < count; ++i) {
            total += asks_[i].quantity_;
        }
        return total;
    }

    void clear() noexcept {
        bids_.fill(PriceLevel{});
        asks_.fill(PriceLevel{});
        isStale_.store(false, std::memory_order_release);
    }

    void markStale() noexcept {
        isStale_.store(true, std::memory_order_release);
    }

    [[nodiscard]] bool isStale() const noexcept {
        return isStale_.load(std::memory_order_acquire);
    }
};

struct alignas(64) Trade {
    uint64_t tradeId_{0};
    uint32_t instrumentId_{0};
    uint32_t venueId_{0};
    Price price_{0};
    Quantity quantity_{0};
    Side side_{Side::BUY};
    Timestamp timestamp_{};
    uint64_t sequence_{0};

    Trade() noexcept = default;

    [[nodiscard]] bool isValid() const noexcept {
        return tradeId_ > 0 && instrumentId_ > 0 && quantity_ > 0 && price_ > 0;
    }

    [[nodiscard]] int64_t notionalValue() const noexcept {
        return static_cast<int64_t>(quantity_) * price_ / PRICE_MULTIPLIER;
    }
};

struct alignas(64) MarketDataEvent {
    static constexpr size_t kMaxTradesPerEvent = 100;

    MarketDataType type_{MarketDataDataType::SNAPSHOT};
    uint32_t instrumentId_{0};
    Timestamp timestamp_{};
    uint64_t sequence_{0};

    union {
        OrderBookLevel2<10>* orderBook_;
        Trade* trade_;
    };

    bool hasOrderBook_{false};
    std::vector<Trade> trades_;

    MarketDataEvent() noexcept : orderBook_(nullptr), hasOrderBook_(false) {}

    ~MarketDataEvent() {
        if (hasOrderBook_ && orderBook_) {
            delete orderBook_;
            orderBook_ = nullptr;
        }
    }

    void setOrderBook(OrderBookLevel2<10>* ob) noexcept {
        orderBook_ = ob;
        hasOrderBook_ = true;
        type_ = MarketDataType::SNAPSHOT;
    }

    void addTrade(const Trade& trade) noexcept {
        if (trades_.size() < kMaxTradesPerEvent) {
            trades_.push_back(trade);
            if (trades_.size() == 1) {
                type_ = MarketDataType::TRADE;
            }
        }
    }

    [[nodiscard]] bool hasData() const noexcept {
        return hasOrderBook_ || !trades_.empty();
    }

    void clear() noexcept {
        if (hasOrderBook_ && orderBook_) {
            delete orderBook_;
            orderBook_ = nullptr;
            hasOrderBook_ = false;
        }
        trades_.clear();
        type_ = MarketDataType::SNAPSHOT;
    }
};

class MarketDataCache {
public:
    static constexpr size_t kMaxInstruments = 10000;

    explicit MarketDataCache() noexcept : cache_(kMaxInstruments) {
        for (auto& ob : cache_) {
            ob = std::make_unique<OrderBookLevel2<10>>();
        }
    }

    [[nodiscard]] OrderBookLevel2<10>* getOrderBook(uint32_t instrumentId) noexcept {
        if (instrumentId < kMaxInstruments) {
            return cache_[instrumentId].get();
        }
        return nullptr;
    }

    [[nodiscard]] const OrderBookLevel2<10>* getOrderBook(uint32_t instrumentId) const noexcept {
        if (instrumentId < kMaxInstruments) {
            return cache_[instrumentId].get();
        }
        return nullptr;
    }

    void updateFromEvent(const MarketDataEvent& event) noexcept {
        if (event.hasOrderBook_ && event.orderBook_) {
            auto* target = getOrderBook(event.instrumentId_);
            if (target) {
                *target = *event.orderBook_;
                target->instrumentId_ = event.instrumentId_;
                target->sequence_.store(event.sequence_, std::memory_order_release);
            }
        }
        for (const auto& trade : event.trades_) {
            processTrade(trade);
        }
    }

private:
    void processTrade(const Trade& trade) noexcept {
        auto* ob = getOrderBook(trade.instrumentId_);
        if (!ob) return;

        ob->updateTimestamp();
        auto seq = ob->sequence_.load(std::memory_order_relaxed);
        ob->sequence_.store(seq + 1, std::memory_order_release);
    }

    std::vector<std::unique_ptr<OrderBookLevel2<10>>> cache_;
};

}
}