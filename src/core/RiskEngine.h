#pragma once

#include <memory>
#include <atomic>
#include <cstdint>
#include <optional>
#include <functional>
#include <vector>
#include <string_view>

#include "domain/MarketData.h"
#include "domain/RiskLimits.h"
#include "domain/VaRModel.h"

namespace risk_engine {
namespace core {

enum class RiskDecision : uint8_t {
    ACCEPT = 0,
    REJECT = 1,
    REDUCE = 2,
    REVIEW = 3
};

enum class CircuitBreakerState : uint8_t {
    CLOSED = 0,
    OPEN = 1,
    HALF_OPEN = 2
};

struct RiskEvaluationResult {
    RiskDecision decision_{RiskDecision::ACCEPT};
    double exposure_{0.0};
    double limit_{0.0};
    double utilizationPct_{0.0};
    CircuitBreakerState circuitBreaker_{CircuitBreakerState::CLOSED};
    bool killSwitchTriggered_{false};
    std::string_view rejectionReason_;
    uint64_t evaluationLatencyNs_{0};
    Timestamp timestamp_{};
};

struct Order {
    uint64_t orderId_{0};
    uint32_t instrumentId_{0};
    uint32_t traderId_{0};
    uint32_t strategyId_{0};
    Side side_{Side::BUY};
    Price price_{0};
    Quantity quantity_{0};
    OrderType type_{OrderType::LIMIT};
    Timestamp timestamp_{};

    [[nodiscard]] bool isValid() const noexcept {
        return orderId_ > 0 && instrumentId_ > 0 && quantity_ > 0 && price_ > 0;
    }

    [[nodiscard]] int64_t notional() const noexcept {
        return static_cast<int64_t>(quantity_) * price_ / PRICE_MULTIPLIER;
    }
};

using RiskEventHandler = std::function<void(const RiskEvaluationResult&)>;

class IRiskEngine {
public:
    virtual ~IRiskEngine() = default;

    [[nodiscard]] virtual RiskEvaluationResult evaluateOrder(const Order& order) = 0;

    virtual void onMarketData(const domain::MarketDataEvent& event) = 0;

    [[nodiscard]] virtual CircuitBreakerState getCircuitBreakerState(uint32_t instrumentId) const = 0;

    [[nodiscard]] virtual bool isKillSwitchActive() const = 0;

    virtual void resetCircuitBreaker(uint32_t instrumentId) = 0;

    virtual void resetKillSwitch() = 0;

    virtual void setRiskEventHandler(RiskEventHandler handler) = 0;

    [[nodiscard]] virtual uint64_t getProcessedOrdersCount() const = 0;

    [[nodiscard]] virtual uint64_t getRejectedOrdersCount() const = 0;

    [[nodiscard]] virtual double getAvgLatencyUs() const = 0;
};

class RiskEngine final : public IRiskEngine {
public:
    explicit RiskEngine(
        std::shared_ptr<domain::MarketDataCache> marketDataCache,
        std::shared_ptr<domain::RiskLimits> riskLimits,
        std::shared_ptr<domain::VaRModel> varModel,
        uint32_t shardId = 0) noexcept;

    RiskEngine(const RiskEngine&) = delete;
    RiskEngine& operator=(const RiskEngine&) = delete;
    RiskEngine(RiskEngine&&) = delete;
    RiskEngine& operator=(RiskEngine&&) = delete;

    [[nodiscard]] RiskEvaluationResult evaluateOrder(const Order& order) override;

    void onMarketData(const domain::MarketDataEvent& event) override;

    [[nodiscard]] CircuitBreakerState getCircuitBreakerState(uint32_t instrumentId) const override;

    [[nodiscard]] bool isKillSwitchActive() const override;

    void resetCircuitBreaker(uint32_t instrumentId) override;

    void resetKillSwitch() override;

    void setRiskEventHandler(RiskEventHandler handler) override;

    [[nodiscard]] uint64_t getProcessedOrdersCount() const override;

    [[nodiscard]] uint64_t getRejectedOrdersCount() const override;

    [[nodiscard]] double getAvgLatencyUs() const override;

private:
    [[nodiscard]] RiskEvaluationResult evaluateRiskLimits(const Order& order) noexcept;

    [[nodiscard]] RiskEvaluationResult evaluateVaR(const Order& order) noexcept;

    [[nodiscard]] RiskEvaluationResult checkCircuitBreaker(uint32_t instrumentId) noexcept;

    void updateCircuitBreakerState(uint32_t instrumentId, bool breached) noexcept;

    void checkKillSwitchAnomaly() noexcept;

    void emitRiskEvent(const RiskEvaluationResult& result) noexcept;

    void recordLatency(uint64_t latencyNs) noexcept;

    std::shared_ptr<domain::MarketDataCache> marketDataCache_;
    std::shared_ptr<domain::RiskLimits> riskLimits_;
    std::shared_ptr<domain::VaRModel> varModel_;

    const uint32_t shardId_;

    struct alignas(64) CircuitBreakerData {
        std::atomic<CircuitBreakerState> state_{CircuitBreakerState::CLOSED};
        std::atomic<uint64_t> breachCount_{0};
        std::atomic<uint64_t> lastBreachTimestamp_{0};
        std::atomic<uint64_t> lastResetTimestamp_{0};

        void reset() noexcept {
            state_.store(CircuitBreakerState::CLOSED, std::memory_order_release);
            breachCount_.store(0, std::memory_order_release);
            lastResetTimestamp_.store(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count(),
                std::memory_order_release);
        }
    };

    static constexpr size_t kMaxInstruments = 10000;
    std::vector<CircuitBreakerData> circuitBreakers_;

    std::atomic<bool> killSwitchActive_{false};
    std::atomic<uint64_t> killSwitchTriggerCount_{0};
    std::atomic<uint64_t> anomalyWindowStart_{0};
    std::atomic<uint64_t> anomalyCount_{0};

    std::atomic<uint64_t> processedOrders_{0};
    std::atomic<uint64_t> rejectedOrders_{0};

    static constexpr size_t kLatencyWindowSize = 1000;
    std::vector<std::atomic<uint64_t>> latencyWindow_;
    std::atomic<size_t> latencyIndex_{0};

    std::optional<RiskEventHandler> eventHandler_;

    static constexpr uint64_t kCircuitBreakerResetTimeoutMs = 5000;
    static constexpr double kCircuitBreakerThresholdMultiplier = 1.5;
    static constexpr uint64_t kAnomalyWindowMs = 60000;
    static constexpr double kAnomalyThresholdDeviation = 3.0;
};

}
}