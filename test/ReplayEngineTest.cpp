#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <filesystem>
#include "src/application/ReplayEngine.h"
#include "src/core/RiskEngine.h"

using namespace risk_engine;
using namespace std::chrono_literals;

TEST_CASE("ReplayEngine reproduce eventos en orden deterministico", "[replay][deterministic]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 100; ++i) {
        RiskEvent event;
        event.eventId = "EVT_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now() + std::chrono::milliseconds(i);
        event.instrumentId = "AAPL";
        event.traderId = "TRADER_REPLAY";
        event.orderId = "ORD_" + std::to_string(i);
        event.riskDecision = (i % 2 == 0) ? RiskDecision::APPROVED : RiskDecision::REJECTED;
        event.exposure = 1000.0 * i;
        event.limit = 50000.0;
        events.push_back(event);
    }

    replayEngine.loadEvents(events);

    auto results = replayEngine.replay();

    REQUIRE(results.size() == 100);
    REQUIRE(results[0].eventId == "EVT_0");
    REQUIRE(results[99].eventId == "EVT_99");
}

TEST_CASE("ReplayEngine genera audit trail MiFID II", "[replay][mifid2]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 10; ++i) {
        RiskEvent event;
        event.eventId = "AUDIT_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "MSFT";
        event.traderId = "TRADER_MIFID";
        event.strategyId = "STRAT_Compliance";
        event.orderId = "ORD_MIFID_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 5000.0;
        event.limit = 10000.0;
        event.circuitBreakerState = CircuitBreakerState::CLOSED;
        events.push_back(event);
    }

    auto auditPath = replayEngine.generateAuditTrail(events);

    REQUIRE(std::filesystem::exists(auditPath));

    std::ifstream auditFile(auditPath);
    std::string line;
    int lineCount = 0;
    while (std::getline(auditFile, line)) {
        lineCount++;
    }
    REQUIRE(lineCount >= 10);
}

TEST_CASE("ReplayEngine verifica consistencia de decisiones", "[replay][consistency]") {
    RiskEngine engine;
    engine.initialize();
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 50; ++i) {
        Order order;
        order.instrumentId = "CONS";
        order.traderId = "TRADER_CONS";
        order.quantity = 100;
        order.price = 100.0;
        order.side = OrderSide::BUY;

        MarketData marketData;
        marketData.instrumentId = "CONS";
        marketData.last = 100.0;
        marketData.timestamp = std::chrono::system_clock::now();

        auto decision = engine.evaluateOrder(order, marketData);

        RiskEvent event;
        event.eventId = "CONS_EVT_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = order.instrumentId;
        event.traderId = order.traderId;
        event.orderId = "ORD_CONS_" + std::to_string(i);
        event.riskDecision = decision.approved ? RiskDecision::APPROVED : RiskDecision::REJECTED;
        event.exposure = decision.exposure;
        event.limit = decision.limit;
        events.push_back(event);
    }

    auto results = replayEngine.replay();

    for (size_t i = 0; i < results.size(); ++i) {
        REQUIRE(results[i].riskDecision == events[i].riskDecision);
    }
}

TEST_CASE("ReplayEngine reconstruye estado del sistema en punto temporal", "[replay][point-in-time]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    auto baseTime = std::chrono::system_clock::now();

    for (int i = 0; i < 30; ++i) {
        RiskEvent event;
        event.eventId = "PIT_" + std::to_string(i);
        event.timestamp = baseTime + std::chrono::seconds(i * 10);
        event.instrumentId = "PIT_INST";
        event.traderId = "TRADER_PIT";
        event.orderId = "ORD_PIT_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 1000.0 * (i + 1);
        event.limit = 50000.0;
        events.push_back(event);
    }

    replayEngine.loadEvents(events);

    auto targetTime = baseTime + std::chrono::seconds(150);
    auto state = replayEngine.reconstructStateAt(targetTime);

    REQUIRE(state.exposure > 0.0);
    REQUIRE(state.eventsProcessed <= 30);
}

TEST_CASE("ReplayEngine detecta anomalias en reproduccion", "[replay][anomaly-detection]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 20; ++i) {
        RiskEvent event;
        event.eventId = "ANOMALY_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "ANOMALY_INST";
        event.traderId = "TRADER_ANOMALY";
        event.orderId = "ORD_ANOMALY_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = (i == 15) ? 1000000.0 : 1000.0;
        event.limit = 50000.0;
        events.push_back(event);
    }

    auto anomalies = replayEngine.detectAnomalies(events);

    REQUIRE(anomalies.size() > 0);
}

TEST_CASE("ReplayEngine soporta replay parcial por rango temporal", "[replay][partial]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    auto baseTime = std::chrono::system_clock::now();

    for (int i = 0; i < 100; ++i) {
        RiskEvent event;
        event.eventId = "PARTIAL_" + std::to_string(i);
        event.timestamp = baseTime + std::chrono::seconds(i);
        event.instrumentId = "PARTIAL_INST";
        event.traderId = "TRADER_PARTIAL";
        event.orderId = "ORD_PARTIAL_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 1000.0;
        event.limit = 50000.0;
        events.push_back(event);
    }

    replayEngine.loadEvents(events);

    auto startTime = baseTime + std::chrono::seconds(30);
    auto endTime = baseTime + std::chrono::seconds(60);

    auto partialResults = replayEngine.replayPartial(startTime, endTime);

    REQUIRE(partialResults.size() > 0);
    REQUIRE(partialResults.size() <= 30);
}

TEST_CASE("ReplayEngine exporta a formato compliance", "[replay][compliance]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 5; ++i) {
        RiskEvent event;
        event.eventId = "COMP_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "COMP_INST";
        event.traderId = "TRADER_COMP";
        event.strategyId = "STRAT_COMP";
        event.orderId = "ORD_COMP_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 5000.0;
        event.limit = 10000.0;
        event.circuitBreakerState = CircuitBreakerState::CLOSED;
        events.push_back(event);
    }

    auto exportPath = replayEngine.exportToComplianceFormat(events);

    REQUIRE(std::filesystem::exists(exportPath));

    std::ifstream file(exportPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    REQUIRE(content.find("timestamp") != std::string::npos);
    REQUIRE(content.find("traderId") != std::string::npos);
    REQUIRE(content.find("riskDecision") != std::string::npos);
}

TEST_CASE("ReplayEngine verifica trazabilidad completa de decisiones", "[replay][traceability]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 15; ++i) {
        RiskEvent event;
        event.eventId = "TRACE_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "TRACE_INST";
        event.traderId = "TRADER_TRACE";
        event.strategyId = "STRAT_TRACE";
        event.orderId = "ORD_TRACE_" + std::to_string(i);
        event.riskDecision = (i % 3 == 0) ? RiskDecision::REJECTED : RiskDecision::APPROVED;
        event.exposure = 1000.0 * i;
        event.limit = 10000.0;
        event.circuitBreakerState = CircuitBreakerState::CLOSED;
        event.marketDataSnapshot.bid = 100.0;
        event.marketDataSnapshot.ask = 100.10;
        event.marketDataSnapshot.last = 100.05;
        events.push_back(event);
    }

    auto trace = replayEngine.generateTraceabilityReport(events);

    REQUIRE(trace.totalEvents == 15);
    REQUIRE(trace.approvedCount > 0);
    REQUIRE(trace.rejectedCount > 0);
    REQUIRE(trace.eventsWithMarketData == 15);
}