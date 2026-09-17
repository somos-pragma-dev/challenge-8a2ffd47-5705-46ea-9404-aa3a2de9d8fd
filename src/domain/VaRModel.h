#ifndef RISK_ENGINE_VAR_MODEL_H
#define RISK_ENGINE_VAR_MODEL_H

#include <atomic>
#include <cmath>
#include <vector>
#include <array>
#include <memory>
#include <optional>
#include <chrono>
#include <algorithm>

namespace risk_engine {

/**
 * Parámetros de calibración para el modelo VaR.
 * Configura los factores de confianza, ventana temporal y decaimiento.
 */
struct VaRCalibrationParams {
    double confidenceLevel;
    int lookbackDays;
    double decayFactor;
    bool useIntradayScaling;
    double intradayScalingFactor;
    
    VaRCalibrationParams() 
        : confidenceLevel(0.99)
        , lookbackDays(250)
        , decayFactor(0.94)
        , useIntradayScaling(true)
        , intradayScalingFactor(1.5) {}
    
    VaRCalibrationParams(double conf, int days, double decay, bool intraday = true, double scale = 1.5)
        : confidenceLevel(conf), lookbackDays(days), decayFactor(decay),
          useIntradayScaling(intraday), intradayScalingFactor(scale) {}
};

/**
 * Observación de precio para el cálculo de returns.
 */
struct PriceObservation {
    double price;
    std::chrono::steady_clock::time_point timestamp;
    double volume;
    
    PriceObservation() : price(0.0), timestamp(std::chrono::steady_clock::now()), volume(0.0) {}
    PriceObservation(double p, auto ts, double v = 0.0) 
        : price(p), timestamp(ts), volume(v) {}
};

/**
 * Resultado del cálculo de VaR.
 */
struct VaRResult {
    double varAmount;
    double confidenceLevel;
    double volatility;
    double expectedReturn;
    std::chrono::steady_clock::time_point calculationTime;
    bool isValid;
    
    VaRResult() 
        : varAmount(0.0), confidenceLevel(0.99), volatility(0.0), 
          expectedReturn(0.0), isValid(false) {}
    
    VaRResult(double var, double conf, double vol, double ret, bool valid)
        : varAmount(var), confidenceLevel(conf), volatility(vol),
          expectedReturn(ret), calculationTime(std::chrono::steady_clock::now()), 
          isValid(valid) {}
};

/**
 * Modelo de Value-at-Risk intraday con calibración dinámica.
 * Implementa el cálculo de VaR usando metodología paramétrica
 * con ajustes por volatilidad en tiempo real.
 */
class VaRModel {
public:
    static constexpr size_t MAX_OBSERVATIONS = 1000;
    static constexpr double MIN_VOLATILITY = 0.0001;
    static constexpr double MAX_VOLATILITY = 5.0;
    
private:
    std::string instrumentId_;
    VaRCalibrationParams params_;
    std::vector<PriceObservation> priceHistory_;
    std::array<double, MAX_OBSERVATIONS> returnsBuffer_;
    std::atomic<size_t> observationCount_{0};
    std::atomic<double> currentVolatility_{0.0};
    std::atomic<double> currentVaR_{0.0};
    std::atomic<bool> calibrated_{false};
    std::chrono::steady_clock::time_point lastUpdate_;
    std::atomic<uint64_t> calculationCount_{0};
    
    // Parámetros EWMA para volatilidad
    double lambda_;
    double riskFactor_;
    
public:
    explicit VaRModel(const std::string& instrumentId, const VaRCalibrationParams& params = {})
        : instrumentId_(instrumentId), 
          params_(params),
          lambda_(params.decayFactor),
          riskFactor_(calculateRiskFactor(params.confidenceLevel)) {
        returnsBuffer_.fill(0.0);
    }
    
    void addObservation(const PriceObservation& obs) {
        if (priceHistory_.empty()) {
            priceHistory_.push_back(obs);
            return;
        }
        
        double prevPrice = priceHistory_.back().price;
        if (prevPrice > 0.0 && obs.price > 0.0) {
            double logReturn = std::log(obs.price / prevPrice);
            size_t idx = observationCount_.load(std::memory_order_relaxed) % MAX_OBSERVATIONS;
            returnsBuffer_[idx] = logReturn;
            observationCount_.fetch_add(1, std::memory_order_relaxed);
            
            currentVolatility_.store(calculateEWMAVolatility(), std::memory_order_release);
            calibrated_.store(true, std::memory_order_release);
        }
        
        priceHistory_.push_back(obs);
        if (priceHistory_.size() > MAX_OBSERVATIONS) {
            priceHistory_.erase(priceHistory_.begin());
        }
        
        lastUpdate_ = std::chrono::steady_clock::now();
    }
    
    VaRResult calculate(double positionSize, double currentPrice) {
        calculationCount_.fetch_add(1, std::memory_order_relaxed);
        
        double volatility = currentVolatility_.load(std::memory_order_acquire);
        if (!calibrated_.load(std::memory_order_acquire) || volatility < MIN_VOLATILITY) {
            return VaRResult(0.0, params_.confidenceLevel, 0.0, 0.0, false);
        }
        
        double scaledVolatility = volatility;
        if (params_.useIntradayScaling) {
            scaledVolatility *= params_.intradayScalingFactor;
        }
        
        double var = calculateParametricVaR(positionSize, currentPrice, scaledVolatility);
        
        // Ajuste por decaimiento exponencial
        double timeDecayFactor = calculateTimeDecay();
        var *= timeDecayFactor;
        
        currentVaR_.store(var, std::memory_order_release);
        
        return VaRResult(
            var,
            params_.confidenceLevel,
            scaledVolatility,
            calculateExpectedReturn(),
            true
        );
    }
    
    double getCurrentVolatility() const {
        return currentVolatility_.load(std::memory_order_acquire);
    }
    
    double getCurrentVaR() const {
        return currentVaR_.load(std::memory_order_acquire);
    }
    
    bool isCalibrated() const {
        return calibrated_.load(std::memory_order_acquire);
    }
    
    void recalibrate(const VaRCalibrationParams& newParams) {
        params_ = newParams;
        lambda_ = newParams.decayFactor;
        riskFactor_ = calculateRiskFactor(newParams.confidenceLevel);
        currentVolatility_.store(calculateEWMAVolatility(), std::memory_order_release);
    }
    
    uint64_t calculationCount() const {
        return calculationCount_.load(std::memory_order_relaxed);
    }
    
    std::string instrumentId() const { return instrumentId_; }
    
    size_t observationCount() const {
        return observationCount_.load(std::memory_order_relaxed);
    }
    
private:
    double calculateRiskFactor(double confidenceLevel) const {
        // Aproximación de quantile para distribución normal
        // Usando la transformación de probit
        if (confidenceLevel >= 0.999) return 3.719;
        if (confidenceLevel >= 0.99) return 2.326;
        if (confidenceLevel >= 0.975) return 1.96;
        if (confidenceLevel >= 0.95) return 1.645;
        return 1.645; // Default 95%
    }
    
    double calculateEWMAVolatility() const {
        size_t count = observationCount_.load(std::memory_order_relaxed);
        if (count < 2) return MIN_VOLATILITY;
        
        double weightedSum = 0.0;
        double weightSum = 0.0;
        double weight = 1.0;
        
        size_t startIdx = (count > MAX_OBSERVATIONS) ? (count % MAX_OBSERVATIONS) : 0;
        size_t iterations = std::min(count, MAX_OBSERVATIONS);
        
        for (size_t i = 0; i < iterations; ++i) {
            size_t idx = (startIdx + i) % MAX_OBSERVATIONS;
            double r = returnsBuffer_[idx];
            weightedSum += weight * r * r;
            weightSum += weight;
            weight *= lambda_;
        }
        
        if (weightSum <= 0.0) return MIN_VOLATILITY;
        
        double variance = weightedSum / weightSum;
        double volatility = std::sqrt(variance);
        
        return std::clamp(volatility, MIN_VOLATILITY, MAX_VOLATILITY);
    }
    
    double calculateParametricVaR(double position, double price, double volatility) const {
        if (price <= 0.0 || volatility <= 0.0) return 0.0;
        
        double positionValue = std::abs(position) * price;
        double varPercentile = riskFactor_ * volatility;
        
        return positionValue * varPercentile;
    }
    
    double calculateExpectedReturn() const {
        size_t count = std::min(observationCount_.load(std::memory_order_relaxed), 
                               static_cast<size_t>(params_.lookbackDays));
        if (count < 2) return 0.0;
        
        double sum = 0.0;
        size_t startIdx = (observationCount_.load(std::memory_order_relaxed) >= count) 
            ? (observationCount_.load(std::memory_order_relaxed) - count) % MAX_OBSERVATIONS 
            : 0;
        
        for (size_t i = 0; i < count; ++i) {
            size_t idx = (startIdx + i) % MAX_OBSERVATIONS;
            sum += returnsBuffer_[idx];
        }
        
        return sum / static_cast<double>(count);
    }
    
    double calculateTimeDecay() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - lastUpdate_).count();
        
        // Decaimiento gradual hasta un máximo de 1.5x después de 4 horas
        double decay = std::pow(lambda_, elapsed / 60.0);
        return std::clamp(decay, 0.5, 1.5);
    }
};

/**
 * Gestor de múltiples modelos VaR por instrumento.
 * Proporciona acceso thread-safe a los modelos y cálculo agregado.
 */
class VaRModelRegistry {
public:
    using ModelMap = std::unordered_map<std::string, std::shared_ptr<VaRModel>>;
    
private:
    ModelMap models_;
    std::atomic<uint64_t> totalCalculations_{0};
    
public:
    VaRModelRegistry() = default;
    
    std::shared_ptr<VaRModel> getOrCreateModel(
        const std::string& instrumentId,
        const VaRCalibrationParams& params = {}
    ) {
        auto it = models_.find(instrumentId);
        if (it != models_.end()) {
            return it->second;
        }
        auto model = std::make_shared<VaRModel>(instrumentId, params);
        models_[instrumentId] = model;
        return model;
    }
    
    VaRResult calculatePortfolioVaR(
        const std::vector<std::pair<std::string, double>>& positions,
        double currentPrice
    ) {
        double totalVaR = 0.0;
        double totalVolatility = 0.0;
        bool allValid = true;
        
        for (const auto& [instrumentId, position] : positions) {
            auto it = models_.find(instrumentId);
            if (it != models_.end() && it->second->isCalibrated()) {
                auto result = it->second->calculate(position, currentPrice);
                if (result.isValid) {
                    totalVaR += result.varAmount;
                    totalVolatility += result.volatility;
                } else {
                    allValid = false;
                }
            } else {
                allValid = false;
            }
        }
        
        totalCalculations_.fetch_add(1, std::memory_order_relaxed);
        
        return VaRResult(totalVaR, 0.99, totalVolatility / positions.size(), 0.0, allValid);
    }
    
    size_t modelCount() const { return models_.size(); }
    
    uint64_t totalCalculations() const {
        return totalCalculations_.load(std::memory_order_relaxed);
    }
    
    void clear() {
        models_.clear();
    }
};

} // namespace risk_engine

#endif // RISK_ENGINE_VAR_MODEL_H