#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <thread>
#include <atomic>
#include "src/core/RiskEngine.h"
#include "src/application/KillSwitch.h"
#include "src/domain/RiskLimits.h"

using namespace risk_engine;
using namespace std::chrono_literals;

TEST_CASE("CircuitBreaker inicia en estado CLOSED", "[circuit-breaker][init]") {
    RiskEngine engine;
    engine.initialize();

    auto state = engine.getCircuitBreakerState("AAPL");
    REQUIRE(state == CircuitBreakerState::CLOSED);
}

TEST_CASE("CircuitBreaker abre cuando se excede umbral", "[circuit-breaker][threshold]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.exposureThresholdMultiplier = 1.0;
    engine.updateLimits(limits);

    for (int i = 0; i < 10; ++i) {
        Order order;
        order.instrumentId = "BTC";
        order.traderId = "TRADER_BTC";
        order.quantity = 1000;
        order.price = 50000.0;
        order.side = OrderSide::BUY;

        MarketData marketData;
        marketData.instrumentId = "BTC";
        marketData.last = 50000.0;
        marketData.timestamp = std::chrono::system_clock::now();

        engine.evaluateOrder(order, marketData);
    }

    auto state = engine.getCircuitBreakerState("BTC");
    REQUIRE(state == CircuitBreakerState::OPEN);
}

TEST_CASE("CircuitBreaker transiciona a HALF_OPEN despues de timeout", "[circuit-breaker][recovery]") {
    RiskEngine engine;
    engine.initialize();

    Order order;
    order.instrumentId = "ETH";
    order.traderId = "TRADER_ETH";
    order.quantity = 5000;
    order.price = 3000.0;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "ETH";
    marketData.last = 3000.0;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 15; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    auto openState = engine.getCircuitBreakerState("ETH");
    REQUIRE(openState == CircuitBreakerState::OPEN);

    std::this_thread::sleep_for(5500ms);

    auto halfOpenState = engine.getCircuitBreakerState("ETH");
    REQUIRE(halfOpenState == CircuitBreakerState::HALF_OPEN);
}

TEST_CASE("CircuitBreaker permite ordenes de prueba en estado HALF_OPEN", "[circuit-breaker][probe]") {
    RiskEngine engine;
    engine.initialize();

    Order order;
    order.instrumentId = "SOL";
    order.traderId = "TRADER_SOL";
    order.quantity = 100;
    order.price = 100.0;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "SOL";
    marketData.last = 100.0;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 20; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    std::this_thread::sleep_for(5500ms);

    auto decision = engine.evaluateOrder(order, marketData);
    auto state = engine.getCircuitBreakerState("SOL");

    REQUIRE((state == CircuitBreakerState::HALF_OPEN || state == CircuitBreakerState::CLOSED));
}

TEST_CASE("CircuitBreaker cierra despues de exitos en HALF_OPEN", "[circuit-breaker][recovery]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.circuitBreakerSuccessThreshold = 3;
    engine.updateLimits(limits);

    Order order;
    order.instrumentId = "DOGE";
    order.traderId = "TRADER_DOGE";
    order.quantity = 10;
    order.price = 0.10;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "DOGE";
    marketData.last = 0.10;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 15; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    std::this_thread::sleep_for(5500ms);

    for (int i = 0; i < 5; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    auto state = engine.getCircuitBreakerState("DOGE");
    REQUIRE(state == CircuitBreakerState::CLOSED);
}

TEST_CASE("KillSwitch activa ante comportamiento anionomo", "[kill-switch][anomaly]") {
    KillSwitch killSwitch;
    killSwitch.initialize();

    std::vector<Order> orders;
    for (int i = 0; i < 100; ++i) {
        Order order;
        order.traderId = "TRADER_ANOMALY";
        order.strategyId = "STRAT_Anomaly";
        order.quantity = 10000 + (i * 100);
        order.price = 100.0;
        order.side = OrderSide::BUY;
        orders.push_back(order);
    }

    auto triggerTime = killSwitch.evaluate(orders);

    REQUIRE(triggerTime.has_value());
}

TEST_CASE("KillSwitch no activa con comportamiento normal", "[kill-switch][normal]") {
    KillSwitch killSwitch;
    killSwitch.initialize();

    std::vector<Order> orders;
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> qtyDist(90, 110);

    for (int i = 0; i < 100; ++i) {
        Order order;
        order.traderId = "TRADER_NORMAL";
        order.strategyId = "STRAT_Normal";
        order.quantity = qtyDist(rng);
        order.price = 100.0;
        order.side = OrderSide::BUY;
        orders.push_back(order);
    }

    auto triggerTime = killSwitch.evaluate(orders);

    REQUIRE(triggerTime.has_value() == false);
}

TEST_CASE("KillSwitch detecta desviacion estandar anomalamente alta", "[kill-switch][deviation]") {
    KillSwitch killSwitch;
    killSwitch.initialize();

    std::vector<Order> orders;
    std::mt19937 rng(99);
    std::normal_distribution<double> qtyDist(1000.0, 500.0);

    for (int i = 0; i < 50; ++i) {
        Order order;
        order.traderId = "TRADER_HIGH_DEV";
        order.strategyId = "STRAT_HighDev";
        order.quantity = static_cast<int>(std::abs(qtyDist(rng)));
        order.price = 100.0;
        order.side = OrderSide::BUY;
        orders.push_back(order);
    }

    auto triggerTime = killSwitch.evaluate(orders);

    REQUIRE(triggerTime.has_value());
}

TEST_CASE("CircuitBreaker con volatilidad extrema abre inmediatamente", "[circuit-breaker][volatility]") {
    RiskEngine engine;
    engine.initialize();

    std::vector<MarketData> extremeVolHistory;
    double price = 100.0;
    for (int i = 0; i < 10; ++i) {
        price *= (1.0 + ((double)rand() / RAND_MAX - 0.5) * 0.30);
        MarketData md;
        md.instrumentId = "VOL_TEST";
        md.last = price;
        md.timestamp = std::chrono::system_clock::now();
        extremeVolHistory.push_back(md);
    }
    engine.updatePriceHistory("VOL_TEST", extremeVolHistory);

    Order order;
    order.instrumentId = "VOL_TEST";
    order.traderId = "TRADER_VOL_EXTREME";
    order.quantity = 100;
    order.price = price;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "VOL_TEST";
    marketData.last = price;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == false);
}

TEST_CASE("CircuitBreaker multiple instruments independientes", "[circuit-breaker][multi]") {
    RiskEngine engine;
    engine.initialize();

    std::vector<std::string> instruments = {"AAA", "BBB", "CCC", "DDD", "EEE"};

    for (const auto& inst : instruments) {
        auto initialState = engine.getCircuitBreakerState(inst);
        REQUIRE(initialState == CircuitBreakerState::CLOSED);
    }

    Order order;
    order.traderId = "TRADER_MULTI";
    order.quantity = 10000;
    order.price = 100.0;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.last = 100.0;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 20; ++i) {
        order.instrumentId = "AAA";
        marketData.instrumentId = "AAA";
        engine.evaluateOrder(order, marketData);
    }

    REQUIRE(engine.getCircuitBreakerState("AAA") == CircuitBreakerState::OPEN);
    REQUIRE(engine.getCircuitBreakerState("BBB") == CircuitBreakerState::CLOSED);
    REQUIRE(engine.getCircuitBreakerState("CCC") == CircuitBreakerState::CLOSED);
}