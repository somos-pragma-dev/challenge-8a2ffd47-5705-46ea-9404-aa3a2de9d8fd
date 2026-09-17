#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <csignal>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sinks.h>
#include <nlohmann/json.hpp>
#include "core/RiskEngine.h"
#include "core/CircuitBreaker.h"
#include "application/KillSwitch.h"
#include "application/ReplayEngine.h"
#include "infrastructure/DisruptorConfig.h"
#include "infrastructure/ShardingStrategy.h"

namespace {
    std::atomic<bool> g_running{true};
    
    void signalHandler(int signal) {
        if (signal == SIGINT || signal == SIGTERM) {
            spdlog::warn("Señal de terminación recibida, shutting down...");
            g_running = false;
        }
    }
}

namespace risk_engine {

void initializeLogging() {
    try {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::info);
        
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/risk_engine.log", 1024 * 1024 * 10, 3
        );
        fileSink->set_level(spdlog::level::debug);
        
        std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
        auto logger = std::make_shared<spdlog::logger>( "risk_engine", begin(sinks), end(sinks) );
        logger->set_level(spdlog::level::debug);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
        
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
        
        SPDLOG_INFO("Logging inicializado: console + rotating file");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Error inicializando logging: " << ex.what() << std::endl;
    }
}

RiskEngineConfig loadConfig(const std::string& configPath) {
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            SPDLOG_WARN("No se pudo abrir {}, usando configuración por defecto", configPath);
            return RiskEngineConfig{};
        }
        
        nlohmann::json configJson;
        configFile >> configJson;
        
        RiskEngineConfig config;
        config.latencyTargetUs = configJson.value("latencyTargetUs", 500);
        config.p99Target = configJson.value("p99Target", 500);
        config.shardingStrategy = configJson.value("shardingStrategy", "byInstrument");
        
        if (configJson.contains("circuitBreaker")) {
            auto cb = configJson["circuitBreaker"];
            config.circuitBreakerEnabled = cb.value("enabled", true);
            config.circuitBreakerThresholdMultiplier = cb.value("thresholdMultiplier", 1.5);
            config.circuitBreakerResetTimeoutMs = cb.value("resetTimeoutMs", 5000);
        }
        
        if (configJson.contains("killSwitch")) {
            auto ks = configJson["killSwitch"];
            config.killSwitchEnabled = ks.value("enabled", true);
            config.killSwitchThresholdDeviation = ks.value("thresholdDeviation", 3.0);
            config.killSwitchWindowMs = ks.value("anomalyDetectionWindowMs", 60000);
        }
        
        SPDLOG_INFO("Configuración cargada desde {}", configPath);
        return config;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error cargando configuración: {}", e.what());
        return RiskEngineConfig{};
    }
}

void runNormalMode(const std::string& configPath) {
    SPDLOG_INFO("=== INICIANDO MODO NORMAL ===");
    
    auto config = loadConfig(configPath);
    auto riskEngine = std::make_unique<RiskEngine>(config);
    
    if (!riskEngine->initialize()) {
        SPDLOG_ERROR("Fallo al inicializar RiskEngine");
        return;
    }
    
    KillSwitchConfig ksConfig;
    ksConfig.enabled = config.killSwitchEnabled;
    ksConfig.thresholdDeviation = config.killSwitchThresholdDeviation;
    ksConfig.anomalyDetectionWindowMs = config.killSwitchWindowMs;
    ksConfig.autoResetTimeoutMs = 300000;
    auto killSwitch = std::make_unique<KillSwitch>(ksConfig);
    
    CircuitBreakerConfig cbConfig;
    cbConfig.enabled = config.circuitBreakerEnabled;
    cbConfig.thresholdMultiplier = config.circuitBreakerThresholdMultiplier;
    cbConfig.resetTimeoutMs = config.circuitBreakerResetTimeoutMs;
    auto circuitBreaker = std::make_unique<CircuitBreaker>(cbConfig);
    
    riskEngine->setKillSwitch(killSwitch.get());
    riskEngine->setCircuitBreaker(circuitBreaker.get());
    
    SPDLOG_INFO("RiskEngine inicializado con latencia objetivo: {}us p99", config.latencyTargetUs);
    SPDLOG_INFO("KillSwitch: {}, CircuitBreaker: {}", 
                 ksConfig.enabled ? "enabled" : "disabled",
                 cbConfig.enabled ? "enabled" : "disabled");
    
    int eventCount = 0;
    auto lastReport = std::chrono::steady_clock::now();
    
    while (g_running) {
        auto now = std::chrono::steady_clock::now();
        
        if (riskEngine->processEvents()) {
            eventCount++;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastReport).count();
        if (elapsed >= 10) {
            auto stats = riskEngine->getStatistics();
            SPDLOG_INFO("Stats: events={}, avgLatency={}us, p99Latency={}us, rejections={}",
                        eventCount, stats.avgLatencyUs, stats.p99LatencyUs, stats.rejectionCount);
            lastReport = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    SPDLOG_INFO("=== MODO NORMAL FINALIZADO ===");
}

void runReplayMode(const std::string& replayFile) {
    SPDLOG_INFO("=== INICIANDO MODO REPLAY ===");
    
    auto config = loadConfig("config/risk_config.json");
    auto riskEngine = std::make_unique<RiskEngine>(config);
    riskEngine->initialize();
    
    ReplayConfig replayConfig;
    replayConfig.replayFilePath = replayFile;
    replayConfig.deterministicMode = true;
    replayConfig.recordEveryEvent = true;
    replayConfig.skipMalformedEvents = false;
    replayConfig.reportPath = "logs/replay_report.json";
    
    auto replayEngine = std::make_unique<ReplayEngine>(replayConfig, std::move(riskEngine));
    
    if (!replayEngine->loadEventsFromFile(replayFile)) {
        SPDLOG_ERROR("Fallo al cargar eventos desde {}", replayFile);
        return;
    }
    
    if (!replayEngine->startReplay()) {
        SPDLOG_ERROR("Fallo al iniciar replay");
        return;
    }
    
    while (replayEngine->isReplaying()) {
        replayEngine->processNextEvent();
        
        if (g_running.load() == false) {
            SPDLOG_WARN("Replay interrumpido por señal");
            break;
        }
    }
    
    SPDLOG_INFO("=== MODO REPLAY FINALIZADO ===");
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --replay <file>    Modo replay determinístico\n";
    std::cout << "  --config <path>   Path a config (default: config/risk_config.json)\n";
    std::cout << "  --help            Mostrar esta ayuda\n";
}

} // namespace risk_engine

int main(int argc, char* argv[]) {
    std::signal(SIGINT, ::signalHandler);
    std::signal(SIGTERM, ::signalHandler);
    
    risk_engine::initializeLogging();
    SPDLOG_INFO("Risk Engine LMAX - Starting...");
    SPDLOG_INFO("C++20 with LMAX Disruptor 4.0");
    
    std::string configPath = "config/risk_config.json";
    std::string replayFile;
    bool replayMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--replay" && i + 1 < argc) {
            replayFile = argv[++i];
            replayMode = true;
        } else if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--help") {
            risk_engine::printUsage(argv[0]);
            return 0;
        }
    }
    
    try {
        if (replayMode) {
            risk_engine::runReplayMode(replayFile);
        } else {
            risk_engine::runNormalMode(configPath);
        }
    } catch (const std::exception& e) {
        SPDLOG_CRITICAL("Excepción no manejada: {}", e.what());
        return 1;
    }
    
    SPDLOG_INFO("Risk Engine terminated gracefully");
    return 0;
}