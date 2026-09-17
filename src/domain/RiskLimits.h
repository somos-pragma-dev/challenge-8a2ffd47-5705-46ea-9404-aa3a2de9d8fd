#ifndef RISK_ENGINE_RISK_LIMITS_H
#define RISK_ENGINE_RISK_LIMITS_H

#include <atomic>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace risk_engine {

/**
 * Estructura que representa un límite de riesgo individual.
 * Cada límite tiene un umbral máximo y un multiplicador de volatilidad.
 */
struct RiskThreshold {
    double maxExposure;
    double volatilityMultiplier;
    bool enabled;
    
    RiskThreshold() : maxExposure(0.0), volatilityMultiplier(1.0), enabled(true) {}
    RiskThreshold(double exposure, double multiplier, bool en = true)
        : maxExposure(exposure), volatilityMultiplier(multiplier), enabled(en) {}
    
    double adjustedLimit(double currentVolatility) const {
        return enabled ? maxExposure * (1.0 + volatilityMultiplier * currentVolatility) : 0.0;
    }
};

/**
 * Límites de riesgo por instrumento financiero.
 * Gestiona los umbrales individuales para cada instrumento.
 */
struct InstrumentLimits {
    RiskThreshold positionLimit;
    RiskThreshold dailyLossLimit;
    RiskThreshold concentrationLimit;
    std::atomic<uint64_t> currentPosition{0};
    std::atomic<double> currentExposure{0.0};
    
    InstrumentLimits() = default;
    
    bool checkPositionLimit(double newPosition, double volatility) const {
        return std::abs(newPosition) <= positionLimit.adjustedLimit(volatility);
    }
    
    bool checkExposureLimit(double exposure, double volatility) const {
        return exposure <= dailyLossLimit.adjustedLimit(volatility);
    }
    
    bool checkConcentration(double portfolioExposure, double totalExposure, double volatility) const {
        if (totalExposure <= 0.0) return true;
        double concentration = portfolioExposure / totalExposure;
        return concentration <= concentrationLimit.adjustedLimit(volatility);
    }
};

/**
 * Límites de riesgo por estrategia de trading.
 * Agrega límites a nivel de estrategia incluyendo límites de VaR.
 */
struct StrategyLimits {
    std::string strategyId;
    RiskThreshold varLimit;
    RiskThreshold dailyPnlLimit;
    RiskThreshold orderRateLimit;
    std::atomic<uint64_t> orderCount{0};
    std::atomic<double> currentVaR{0.0};
    std::atomic<double> dailyPnl{0.0};
    std::chrono::steady_clock::time_point windowStart;
    
    explicit StrategyLimits(const std::string& id) 
        : strategyId(id), windowStart(std::chrono::steady_clock::now()) {}
    
    bool checkVaRLimit(double var, double volatility) const {
        return var <= varLimit.adjustedLimit(volatility);
    }
    
    bool checkPnlLimit(double pnl, double volatility) const {
        return std::abs(pnl) <= dailyPnlLimit.adjustedLimit(volatility);
    }
    
    bool checkOrderRate(uint64_t newOrderCount, double volatility) const {
        return newOrderCount <= static_cast<uint64_t>(orderRateLimit.adjustedLimit(volatility));
    }
    
    void resetWindow() {
        windowStart = std::chrono::steady_clock::now();
        orderCount.store(0);
    }
};

/**
 * Límites de riesgo por trader.
 * Coordina límites individuales, de estrategia y de instrumento.
 */
struct TraderLimits {
    std::string traderId;
    RiskThreshold totalExposureLimit;
    RiskThreshold marginUtilizationLimit;
    std::unordered_map<std::string, std::shared_ptr<StrategyLimits>> strategyLimits;
    std::unordered_map<std::string, InstrumentLimits> instrumentLimits;
    std::atomic<bool> blocked{false};
    std::atomic<uint64_t> violationCount{0};
    
    explicit TraderLimits(const std::string& id) : traderId(id) {}
    
    bool isBlocked() const { return blocked.load(std::memory_order_acquire); }
    
    void block() { 
        blocked.store(true, std::memory_order_release);
        violationCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    void unblock() { blocked.store(false, std::memory_order_release); }
    
    bool checkTotalExposure(double totalExposure, double volatility) const {
        return totalExposure <= totalExposureLimit.adjustedLimit(volatility);
    }
    
    void addStrategy(const std::string& strategyId) {
        strategyLimits[strategyId] = std::make_shared<StrategyLimits>(strategyId);
    }
    
    std::shared_ptr<StrategyLimits> getStrategyLimits(const std::string& strategyId) const {
        auto it = strategyLimits.find(strategyId);
        return (it != strategyLimits.end()) ? it->second : nullptr;
    }
    
    InstrumentLimits& getInstrumentLimits(const std::string& instrumentId) {
        return instrumentLimits[instrumentId];
    }
};

/**
 * Gestor central de límites de riesgo.
 * Mantiene el estado de todos los límites y proporciona validación thread-safe.
 */
class RiskLimitsManager {
public:
    using TraderLimitsMap = std::unordered_map<std::string, std::shared_ptr<TraderLimits>>;
    
private:
    TraderLimitsMap traders_;
    std::atomic<uint64_t> globalViolationCount_{0};
    
public:
    RiskLimitsManager() = default;
    
    std::shared_ptr<TraderLimits> getOrCreateTrader(const std::string& traderId) {
        auto it = traders_.find(traderId);
        if (it != traders_.end()) {
            return it->second;
        }
        auto trader = std::make_shared<TraderLimits>(traderId);
        traders_[traderId] = trader;
        return trader;
    }
    
    bool validateTraderExposure(
        const std::string& traderId,
        const std::string& strategyId,
        const std::string& instrumentId,
        double exposure,
        double volatility
    ) {
        auto trader = getOrCreateTrader(traderId);
        
        if (trader->isBlocked()) {
            globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        
        if (!trader->checkTotalExposure(exposure, volatility)) {
            trader->block();
            globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        
        auto strategyLimits = trader->getStrategyLimits(strategyId);
        if (strategyLimits) {
            double currentVaR = strategyLimits->currentVaR.load(std::memory_order_acquire);
            if (!strategyLimits->checkVaRLimit(currentVaR, volatility)) {
                trader->block();
                globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
                return false;
            }
        }
        
        auto& instrumentLimits = trader->getInstrumentLimits(instrumentId);
        if (!instrumentLimits.checkExposureLimit(exposure, volatility)) {
            trader->block();
            globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        
        return true;
    }
    
    uint64_t globalViolationCount() const {
        return globalViolationCount_.load(std::memory_order_relaxed);
    }
    
    void resetViolations() {
        globalViolationCount_.store(0, std::memory_order_relaxed);
        for (auto& [id, trader] : traders_) {
            trader->violationCount.store(0, std::memory_order_relaxed);
            trader->unblock();
        }
    }
    
    size_t traderCount() const { return traders_.size(); }
};

} // namespace risk_engine

#endif // RISK_ENGINE_RISK_LIMITS_H