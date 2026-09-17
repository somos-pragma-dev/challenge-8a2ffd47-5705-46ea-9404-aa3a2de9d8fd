#include "application/ReplayEngine.h"
#include "core/RiskEngine.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace risk_engine {

ReplayEngine::ReplayEngine(const ReplayConfig& config, std::shared_ptr<RiskEngine> riskEngine)
    : config_(config)
    , riskEngine_(riskEngine)
    , isReplaying_(false)
    , currentEventIndex_(0)
    , playbackSpeed_(1.0) {
    SPDLOG_INFO("ReplayEngine inicializado con config: replayFile={}, deterministic={}",
                config.replayFilePath, config.deterministicMode ? "true" : "false");
}

bool ReplayEngine::loadEventsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        SPDLOG_ERROR("No se pudo abrir el archivo de replay: {}", filePath);
        return false;
    }
    
    events_.clear();
    std::string line;
    int64_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        try {
            auto event = parseEvent(line, lineNumber);
            if (event) {
                events_.push_back(*event);
            }
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Error parseando línea {}: {}", lineNumber, e.what());
            if (!config_.skipMalformedEvents) {
                return false;
            }
        }
    }
    
    SPDLOG_INFO("Cargados {} eventos desde {}", events_.size(), filePath);
    return true;
}

std::optional<ReplayEvent> ReplayEngine::parseEvent(const std::string& line, int64_t lineNumber) {
    std::istringstream iss(line);
    std::string timestampStr, eventType, payload;
    
    if (!std::getline(iss, timestampStr, '|')) return std::nullopt;
    if (!std::getline(iss, eventType, '|')) return std::nullopt;
    if (!std::getline(iss, payload)) return std::nullopt;
    
    ReplayEvent event;
    event.lineNumber = lineNumber;
    event.timestamp = std::stoll(timestampStr);
    event.eventType = eventType;
    event.payload = payload;
    event.replayed = false;
    event.replayResult = "";
    
    return event;
}

bool ReplayEngine::startReplay() {
    if (events_.empty()) {
        SPDLOG_ERROR("No hay eventos para reproducir");
        return false;
    }
    
    if (config_.deterministicMode) {
        std::sort(events_.begin(), events_.end(), 
            [](const ReplayEvent& a, const ReplayEvent& b) {
                return a.timestamp < b.timestamp;
            });
        SPDLOG_INFO("Eventos ordenados determinísticamente por timestamp");
    }
    
    isReplaying_ = true;
    currentEventIndex_ = 0;
    startReplayTimestamp_ = currentTimestampMillis();
    
    SPDLOG_INFO("Iniciando replay de {} eventos", events_.size());
    emitAuditEvent("REPLAY_STARTED", "");
    
    return true;
}

bool ReplayEngine::processNextEvent() {
    if (!isReplaying_ || currentEventIndex_ >= events_.size()) {
        return false;
    }
    
    auto& event = events_[currentEventIndex_];
    
    if (config_.deterministicMode) {
        int64_t expectedTime = event.timestamp;
        int64_t currentTime = currentTimestampMillis();
        int64_t targetTime = startReplayTimestamp_ + static_cast<int64_t>(
            (expectedTime - events_[0].timestamp) / playbackSpeed_
        );
        
        while (currentTime < targetTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            currentTime = currentTimestampMillis();
        }
    }
    
    bool success = executeEvent(event);
    event.replayed = true;
    event.replayResult = success ? "SUCCESS" : "FAILURE";
    
    if (config_.recordEveryEvent) {
        emitAuditEvent("REPLAY_EVENT", 
            "type=" + event.eventType + 
            ", line=" + std::to_string(event.lineNumber) + 
            ", result=" + event.replayResult);
    }
    
    currentEventIndex_++;
    
    if (currentEventIndex_ >= events_.size()) {
        finishReplay();
    }
    
    return success;
}

bool ReplayEngine::executeEvent(const ReplayEvent& event) {
    try {
        if (event.eventType == "ORDER_SUBMITTED") {
            return replayOrderSubmission(event);
        } else if (event.eventType == "MARKET_DATA") {
            return replayMarketDataUpdate(event);
        } else if (event.eventType == "RISK_CHECK") {
            return replayRiskCheck(event);
        } else if (event.eventType == "CIRCUIT_BREAKER") {
            return replayCircuitBreaker(event);
        } else {
            SPDLOG_WARN("Tipo de evento desconocido: {}", event.eventType);
            return false;
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error ejecutando evento {}: {}", event.lineNumber, e.what());
        return false;
    }
}

bool ReplayEngine::replayOrderSubmission(const ReplayEvent& event) {
    if (!riskEngine_) {
        SPDLOG_ERROR("RiskEngine no disponible para replay");
        return false;
    }
    
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string orderId = payload.value("orderId", "");
    std::string instrumentId = payload.value("instrumentId", "");
    double quantity = payload.value("quantity", 0.0);
    double price = payload.value("price", 0.0);
    std::string traderId = payload.value("traderId", "");
    std::string strategyId = payload.value("strategyId", "");
    
    RiskDecision decision = riskEngine_->evaluateOrderRisk(
        orderId, instrumentId, quantity, price, traderId, strategyId
    );
    
    SPDLOG_DEBUG("Replay order {} -> decision: {}", orderId, 
                 decision == RiskDecision::APPROVED ? "APPROVED" : "REJECTED");
    
    return true;
}

bool ReplayEngine::replayMarketDataUpdate(const ReplayEvent& event) {
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string instrumentId = payload.value("instrumentId", "");
    double bidPrice = payload.value("bidPrice", 0.0);
    double askPrice = payload.value("askPrice", 0.0);
    int64_t bidSize = payload.value("bidSize", 0);
    int64_t askSize = payload.value("askSize", 0);
    
    if (riskEngine_) {
        riskEngine_->updateMarketData(instrumentId, bidPrice, askPrice, bidSize, askSize);
    }
    
    return true;
}

bool ReplayEngine::replayRiskCheck(const ReplayEvent& event) {
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string traderId = payload.value("traderId", "");
    double exposure = payload.value("exposure", 0.0);
    double limit = payload.value("limit", 0.0);
    
    if (riskEngine_) {
        bool withinLimit = riskEngine_->checkTraderLimit(traderId, exposure, limit);
        SPDLOG_DEBUG("Replay risk check: trader={}, exposure={}, limit={}, within={}",
                     traderId, exposure, limit, withinLimit);
    }
    
    return true;
}

bool ReplayEngine::replayCircuitBreaker(const ReplayEvent& event) {
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string instrumentId = payload.value("instrumentId", "");
    std::string state = payload.value("state", "");
    
    SPDLOG_DEBUG("Replay circuit breaker: instrument={}, state={}", instrumentId, state);
    
    return true;
}

void ReplayEngine::finishReplay() {
    isReplaying_ = false;
    
    int successCount = 0;
    int failureCount = 0;
    for (const auto& event : events_) {
        if (event.replayed) {
            if (event.replayResult == "SUCCESS") {
                successCount++;
            } else {
                failureCount++;
            }
        }
    }
    
    SPDLOG_INFO("Replay terminado: {} eventos, {} exitos, {} fallidos",
                events_.size(), successCount, failureCount);
    
    emitAuditEvent("REPLAY_COMPLETED", 
        "total=" + std::to_string(events_.size()) + 
        ", success=" + std::to_string(successCount) + 
        ", failure=" + std::to_string(failureCount));
    
    generateReplayReport();
}

void ReplayEngine::generateReplayReport() const {
    std::string reportPath = config_.reportPath.empty() ? 
        "logs/replay_report.json" : config_.reportPath;
    
    nlohmann::json report;
    report["totalEvents"] = events_.size();
    report["replayedEvents"] = std::count_if(events_.begin(), events_.end(),
        [](const ReplayEvent& e) { return e.replayed; });
    report["deterministicMode"] = config_.deterministicMode;
    report["playbackSpeed"] = playbackSpeed_;
    
    nlohmann::json eventsJson = nlohmann::json::array();
    for (const auto& event : events_) {
        eventsJson.push_back({
            {"lineNumber", event.lineNumber},
            {"timestamp", event.timestamp},
            {"eventType", event.eventType},
            {"replayed", event.replayed},
            {"result", event.replayResult}
        });
    }
    report["events"] = eventsJson;
    
    std::ofstream outFile(reportPath);
    if (outFile.is_open()) {
        outFile << report.dump(2);
        outFile.close();
        SPDLOG_INFO("Reporte de replay generado: {}", reportPath);
    } else {
        SPDLOG_ERROR("No se pudo generar el reporte de replay: {}", reportPath);
    }
}

void ReplayEngine::emitAuditEvent(const std::string& eventType, const std::string& details) const {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    
    SPDLOG_INFO("MiFID2_AUDIT_REPLAY: timestamp={}, eventType={}, details={}, eventIndex={}/{}",
                timestamp, eventType, details, currentEventIndex_, events_.size());
}

int64_t ReplayEngine::currentTimestampMillis() const {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
}

void ReplayEngine::setPlaybackSpeed(double speed) {
    playbackSpeed_ = std::max(0.1, std::min(10.0, speed));
    SPDLOG_INFO("Playback speed ajustado a {:.1f}x", playbackSpeed_);
}

bool ReplayEngine::isReplaying() const {
    return isReplaying_;
}

size_t ReplayEngine::getEventCount() const {
    return events_.size();
}

size_t ReplayEngine::getProcessedEventCount() const {
    return currentEventIndex_;
}

} // namespace risk_engine