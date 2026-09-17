#include "RiskEngine.h"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <stdexcept>

#include <spdlog/spdlog.h>

namespace risk_engine {
namespace core {

namespace {

constexpr std::string_view kRejectionReasonLimitExceeded{"LIMIT_EXCEEDED"};
constexpr std::string_view kRejectionReasonCircuitBreaker{"CIRCUIT_BREAKER_OPEN"};
constexpr std::string_view kRejectionReasonVaRBreach{"VAR_LIMIT_BREACH"};
constexpr std::string_view kRejectionReasonKillSwitch{"KILL_SWITCH_TRIGGERED"};
constexpr std::string_view kRejectionReasonInvalidOrder{"INVALID_ORDER"};
constexpr std::string_view kRejectionReasonNoLiquidity{"NO_LIQUIDITY"};

}

RiskEngine::RiskEngine(
    std::shared_ptr<domain::MarketDataCache> marketDataCache,
    std::shared_ptr<domain::RiskLimits> riskLimits,
    std::shared_ptr<domain::VaRModel> varModel,
    uint32_t shardId) noexcept
    : marketDataCache_(std::move(marketDataCache))
    , riskLimits_(std::move(riskLimits))
    , varModel_(std::move(varModel))
    , shardId_(shardId)
    , circuitBreakers_(kMaxInstruments)
    , latencyWindow_(kLatencyWindowSize)
{
    if (!marketDataCache_ || !riskLimits_ || !varModel_) {
        throw std::invalid_argument("RiskEngine requires non-null dependencies");
    }

    std::fill(latencyWindow_.begin(), latencyWindow_.end(), 0);

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    anomalyWindowStart_.store(now, std::memory_order_release);

    spdlog::info("RiskEngine initialized on shard {} with {} circuit breakers",
                 shardId_, kMaxInstruments);
}

RiskEvaluationResult RiskEngine::evaluateOrder(const Order& order) {
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!order.isValid()) {
        RiskEvaluationResult result;
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonInvalidOrder;
        result.timestamp_ = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        recordLatency(0);
        return result;
    }

    if (killSwitchActive_.load(std::memory_order_acquire)) {
        RiskEvaluationResult result;
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonKillSwitch;
        result.killSwitchTriggered_ = true;
        result.timestamp_ = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        recordLatency(0);
        emitRiskEvent(result);
        return result;
    }

    auto cbResult = checkCircuitBreaker(order.instrumentId_);
    if (cbResult.decision_ == RiskDecision::REJECT) {
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        auto endTime = std::chrono::high_resolution_clock::now();
        cbResult.evaluationLatencyNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTime - startTime).count();
        recordLatency(cbResult.evaluationLatencyNs_);
        emitRiskEvent(cbResult);
        return cbResult;
    }

    auto limitResult = evaluateRiskLimits(order);
    if (limitResult.decision_ == RiskDecision::REJECT) {
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        auto endTime = std::chrono::high_resolution_clock::now();
        limitResult.evaluationLatencyNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTime - startTime).count();
        recordLatency(limitResult.evaluationLatencyNs_);
        emitRiskEvent(limitResult);
        return limitResult;
    }

    auto varResult = evaluateVaR(order);
    if (varResult.decision_ == RiskDecision::REJECT) {
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        auto endTime = std::chrono::high_resolution_clock::now();
        varResult.evaluationLatencyNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTime - startTime).count();
        recordLatency(varResult.evaluationLatencyNs_);
        emitRiskEvent(varResult);
        return varResult;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto latencyNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

    RiskEvaluationResult result;
    result.decision_ = RiskDecision::ACCEPT;
    result.exposure_ = limitResult.exposure_;
    result.limit_ = limitResult.limit_;
    result.utilizationPct_ = limitResult.utilizationPct_;
    result.evaluationLatencyNs_ = latencyNs;
    result.timestamp_ = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    processedOrders_.fetch_add(1, std::memory_order_relaxed);

    checkKillSwitchAnomaly();

    recordLatency(latencyNs);
    emitRiskEvent(result);

    return result;
}

void RiskEngine::onMarketData(const domain::MarketDataEvent& event) {
    if (marketDataCache_) {
        marketDataCache_->updateFromEvent(event);
    }

    if (varModel_) {
        varModel_->updateVolatility(event.instrumentId_, event.timestamp_);
    }
}

CircuitBreakerState RiskEngine::getCircuitBreakerState(uint32_t instrumentId) const {
    if (instrumentId >= kMaxInstruments) {
        return CircuitBreakerState::OPEN;
    }
    return circuitBreakers_[instrumentId].state_.load(std::memory_order_acquire);
}

bool RiskEngine::isKillSwitchActive() const {
    return killSwitchActive_.load(std::memory_order_acquire);
}

void RiskEngine::resetCircuitBreaker(uint32_t instrumentId) {
    if (instrumentId < kMaxInstruments) {
        circuitBreakers_[instrumentId].reset();
        spdlog::info("Circuit breaker reset for instrument {}", instrumentId);
    }
}

void RiskEngine::resetKillSwitch() {
    killSwitchActive_.store(false, std::memory_order_release);
    killSwitchTriggerCount_.store(0, std::memory_order_release);

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    anomalyWindowStart_.store(now, std::memory_order_release);
    anomalyCount_.store(0, std::memory_order_release);

    spdlog::warn("Kill switch reset");
}

void RiskEngine::setRiskEventHandler(RiskEventHandler handler) {
    eventHandler_ = std::move(handler);
}

uint64_t RiskEngine::getProcessedOrdersCount() const {
    return processedOrders_.load(std::memory_order_relaxed);
}

uint64_t RiskEngine::getRejectedOrdersCount() const {
    return rejectedOrders_.load(std::memory_order_relaxed);
}

double RiskEngine::getAvgLatencyUs() const {
    uint64_t total = 0;
    for (const auto& lat : latencyWindow_) {
        total += lat.load(std::memory_order_relaxed);
    }
    return static_cast<double>(total) / kLatencyWindowSize / 1000.0;
}

RiskEvaluationResult RiskEngine::evaluateRiskLimits(const Order& order) noexcept {
    RiskEvaluationResult result;

    auto limit = riskLimits_->getLimit(order.traderId_, order.strategyId_, order.instrumentId_);
    if (!limit.has_value()) {
        result.decision_ = RiskDecision::ACCEPT;
        return result;
    }

    double currentExposure = riskLimits_->getCurrentExposure(order.traderId_, order.strategyId_, order.instrumentId_);
    double orderNotional = static_cast<double>(order.notional()) / 1000000.0;
    double newExposure = currentExposure + orderNotional;

    result.exposure_ = newExposure;
    result.limit_ = limit.value();
    result.utilizationPct_ = (newExposure / limit.value()) * 100.0;

    if (newExposure > limit.value()) {
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonLimitExceeded;

        updateCircuitBreakerState(order.instrumentId_, true);
    } else if (newExposure > limit.value() * 0.8) {
        result.decision_ = RiskDecision::REVIEW;
    }

    return result;
}

RiskEvaluationResult RiskEngine::evaluateVaR(const Order& order) noexcept {
    RiskEvaluationResult result;

    if (!varModel_ || !marketDataCache_) {
        result.decision_ = RiskDecision::ACCEPT;
        return result;
    }

    auto* orderBook = marketDataCache_->getOrderBook(order.instrumentId_);
    if (!orderBook || orderBook->isStale()) {
        result.decision_ = RiskDecision::REVIEW;
        result.rejectionReason_ = kRejectionReasonNoLiquidity;
        return result;
    }

    double currentVaR = varModel_->calculateVaR(order.instrumentId_);
    double orderNotional = static_cast<double>(order.notional()) / 1000000.0;
    double potentialLoss = varModel_->estimatePotentialLoss(order.instrumentId_, orderNotional);

    if (currentVaR + potentialLoss > varModel_->getVaRLimit()) {
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonVaRBreach;
    }

    return result;
}

RiskEvaluationResult RiskEngine::checkCircuitBreaker(uint32_t instrumentId) noexcept {
    RiskEvaluationResult result;

    if (instrumentId >= kMaxInstruments) {
        result.decision_ = RiskDecision::REJECT;
        result.circuitBreaker_ = CircuitBreakerState::OPEN;
        result.rejectionReason_ = kRejectionReasonCircuitBreaker;
        return result;
    }

    auto& cb = circuitBreakers_[instrumentId];
    auto state = cb.state_.load(std::memory_order_acquire);

    if (state == CircuitBreakerState::OPEN) {
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        auto lastReset = cb.lastResetTimestamp_.load(std::memory_order_relaxed);

        if (now - lastReset > kCircuitBreakerResetTimeoutMs) {
            cb.state_.store(CircuitBreakerState::HALF_OPEN, std::memory_order_release);
            state = CircuitBreakerState::HALF_OPEN;
            spdlog::info("Circuit breaker for instrument {} moved to HALF_OPEN", instrumentId);
        } else {
            result.decision_ = RiskDecision::REJECT;
            result.circuitBreaker_ = CircuitBreakerState::OPEN;
            result.rejectionReason_ = kRejectionReasonCircuitBreaker;
            return result;
        }
    }

    result.circuitBreaker_ = state;
    return result;
}

void RiskEngine::updateCircuitBreakerState(uint32_t instrumentId, bool breached) noexcept {
    if (instrumentId >= kMaxInstruments) return;

    auto& cb = circuitBreakers_[instrumentId];
    auto state = cb.state_.load(std::memory_order_acquire);

    if (breached) {
        cb.breachCount_.fetch_add(1, std::memory_order_relaxed);
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        cb.lastBreachTimestamp_.store(now, std::memory_order_release);

        if (state == CircuitBreakerState::CLOSED) {
            auto breaches = cb.breachCount_.load(std::memory_order_relaxed);
            if (breaches >= 3) {
                cb.state_.store(CircuitBreakerState::OPEN, std::memory_order_release);
                spdlog::warn("Circuit breaker OPEN for instrument {} after {} breaches",
                            instrumentId, breaches);
            }
        } else if (state == CircuitBreakerState::HALF_OPEN) {
            cb.state_.store(CircuitBreakerState::OPEN, std::memory_order_release);
            spdlog::warn("Circuit breaker reopened for instrument {}", instrumentId);
        }
    } else if (state == CircuitBreakerState::HALF_OPEN) {
        cb.reset();
        spdlog::info("Circuit breaker CLOSED for instrument {} after successful check", instrumentId);
    }
}

void RiskEngine::checkKillSwitchAnomaly() noexcept {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    auto windowStart = anomalyWindowStart_.load(std::memory_order_relaxed);
    if (now - windowStart > kAnomalyWindowMs) {
        auto currentCount = anomalyCount_.load(std::memory_order_relaxed);

        if (currentCount > kAnomalyThresholdDeviation * 10) {
            if (!killSwitchActive_.load(std::memory_order_acquire)) {
                killSwitchActive_.store(true, std::memory_order_release);
                killSwitchTriggerCount_.fetch_add(1, std::memory_order_relaxed);
                spdlog::error("KILL SWITCH TRIGGERED: {} anomalies in {}ms window",
                             currentCount, kAnomalyWindowMs);
            }
        }

        anomalyWindowStart_.store(now, std::memory_order_release);
        anomalyCount_.store(0, std::memory_order_release);
    } else {
        auto processed = processedOrders_.load(std::memory_order_relaxed);
        double rate = static_cast<double>(processed) / ((now - windowStart) / 1000.0);

        if (rate > 10000.0) {
            anomalyCount_.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

void RiskEngine::emitRiskEvent(const RiskEvaluationResult& result) noexcept {
    if (eventHandler_) {
        eventHandler_(result);
    }
}

void RiskEngine::recordLatency(uint64_t latencyNs) noexcept {
    auto index = latencyIndex_.load(std::memory_order_relaxed);
    latencyWindow_[index].store(latencyNs, std::memory_order_relaxed);
    latencyIndex_.store((index + 1) % kLatencyWindowSize, std::memory_order_relaxed);
}

}
}