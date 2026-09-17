#include "application/KillSwitch.h"
#include "core/RiskEngine.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace risk_engine {

KillSwitch::KillSwitch(const KillSwitchConfig& config)
    : config_(config)
    , state_(KillSwitchState::CLOSED)
    , anomalyScore_(0.0)
    , lastResetTime_(std::chrono::steady_clock::now()) {
    SPDLOG_INFO("KillSwitch inicializado con ventana={}ms, threshold={}",
                config.anomalyDetectionWindowMs, config.thresholdDeviation);
}

void KillSwitch::recordLatencySample(int64_t latencyMicros) {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(mutex_);
    
    latencyHistory_.push_back({now, latencyMicros});
    
    auto windowStart = now - std::chrono::milliseconds(config_.anomalyDetectionWindowMs);
    latencyHistory_.erase(
        std::remove_if(latencyHistory_.begin(), latencyHistory_.end(),
            [windowStart](const LatencySample& s) { return s.timestamp < windowStart; }),
        latencyHistory_.end()
    );
    
    evaluateAnomalyScore();
}

void KillSwitch::recordErrorRate(double errorRate) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorRateHistory_.push_back(errorRate);
    
    if (errorRateHistory_.size() > 100) {
        errorRateHistory_.erase(errorRateHistory_.begin());
    }
    
    evaluateAnomalyScore();
}

void KillSwitch::recordRejectionRate(double rejectionRate) {
    std::lock_guard<std::mutex> lock(mutex_);
    rejectionRateHistory_.push_back(rejectionRate);
    
    if (rejectionRateHistory_.size() > 100) {
        rejectionRateHistory_.erase(rejectionRateHistory_.begin());
    }
    
    evaluateAnomalyScore();
}

void KillSwitch::evaluateAnomalyScore() {
    if (latencyHistory_.empty()) {
        return;
    }
    
    std::vector<int64_t> latencies;
    latencies.reserve(latencyHistory_.size());
    for (const auto& sample : latencyHistory_) {
        latencies.push_back(sample.latencyMicros);
    }
    
    double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double sqSum = 0.0;
    for (int64_t l : latencies) {
        sqSum += (l - mean) * (l - mean);
    }
    double stdDev = std::sqrt(sqSum / latencies.size());
    
    double latencyAnomalyScore = (stdDev > 0) ? (stdDev / (mean + 1)) : 0.0;
    
    double errorAnomalyScore = 0.0;
    if (!errorRateHistory_.empty()) {
        double meanError = std::accumulate(errorRateHistory_.begin(), errorRateHistory_.end(), 0.0) / errorRateHistory_.size();
        errorAnomalyScore = std::min(meanError * 10.0, 1.0);
    }
    
    double rejectionAnomalyScore = 0.0;
    if (!rejectionRateHistory_.empty()) {
        double meanRejection = std::accumulate(rejectionRateHistory_.begin(), rejectionRateHistory_.end(), 0.0) / rejectionRateHistory_.size();
        rejectionAnomalyScore = std::min(meanRejection * 5.0, 1.0);
    }
    
    anomalyScore_ = (latencyAnomalyScore * 0.5) + (errorAnomalyScore * 0.3) + (rejectionAnomalyScore * 0.2);
    
    SPDLOG_DEBUG("Anomaly score actualizado: latency={:.3f}, error={:.3f}, rejection={:.3f}, total={:.3f}",
                 latencyAnomalyScore, errorAnomalyScore, rejectionAnomalyScore, anomalyScore_);
    
    if (anomalyScore_ > config_.thresholdDeviation && state_ == KillSwitchState::CLOSED) {
        triggerKillSwitch("Anomaly score exceeded threshold");
    }
}

void KillSwitch::triggerKillSwitch(const std::string& reason) {
    state_ = KillSwitchState::OPEN;
    SPDLOG_WARN("KILL SWITCH ACTIVADO: {}", reason);
    SPDLOG_WARN("Anomaly score: {:.3f}, Threshold: {:.3f}", anomalyScore_, config_.thresholdDeviation);
    
    emitAuditEvent("KILL_SWITCH_TRIGGERED", reason);
}

void KillSwitch::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    state_ = KillSwitchState::CLOSED;
    anomalyScore_ = 0.0;
    latencyHistory_.clear();
    errorRateHistory_.clear();
    rejectionRateHistory_.clear();
    lastResetTime_ = std::chrono::steady_clock::now();
    
    SPDLOG_INFO("KillSwitch reseteado manualmente");
    emitAuditEvent("KILL_SWITCH_RESET", "Manual reset");
}

KillSwitchState KillSwitch::getState() const {
    return state_;
}

double KillSwitch::getAnomalyScore() const {
    return anomalyScore_;
}

bool KillSwitch::shouldBlockOrders() const {
    return state_ == KillSwitchState::OPEN;
}

void KillSwitch::emitAuditEvent(const std::string& eventType, const std::string& details) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    
    SPDLOG_INFO("MiFID2_AUDIT: timestamp={}, eventType={}, details={}, state={}, anomalyScore={:.3f}",
                timestamp, eventType, details, 
                state_ == KillSwitchState::OPEN ? "OPEN" : "CLOSED",
                anomalyScore_);
}

void KillSwitch::autoResetIfAllowed() {
    if (state_ != KillSwitchState::OPEN) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastResetTime_
    ).count();
    
    if (elapsed >= config_.autoResetTimeoutMs) {
        state_ = KillSwitchState::HALF_OPEN;
        SPDLOG_INFO("KillSwitch transitando a estado HALF_OPEN tras timeout de {}ms", elapsed);
        emitAuditEvent("KILL_SWITCH_HALF_OPEN", "Auto transition after timeout");
    }
}

} // namespace risk_engine