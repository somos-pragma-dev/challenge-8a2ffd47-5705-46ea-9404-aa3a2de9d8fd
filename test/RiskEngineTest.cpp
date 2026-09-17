#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <chrono>
#include <vector>
#include <random>
#include "src/core/RiskEngine.h"
#include "src/domain/MarketData.h"
#include "src/domain/RiskLimits.h"

using namespace risk_engine;
using namespace std::chrono_literals;

TEST_CASE("RiskEngine evalua orden dentro de limites", "[risk-engine][unit]") {
    RiskEngine engine;
    engine.initialize();

    Order order;
    order.instrumentId = "AAPL";
    order.traderId = "TRADER_001";
    order.strategyId = "STRAT_Momentum";
    order.side = OrderSide::BUY;
    order.quantity = 100;
    order.price = 150.50;

    MarketData marketData;
    marketData.instrumentId = "AAPL";
    marketData.bid = 150.45;
    marketData.ask = 150.55;
    marketData.last = 150.50;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == true);
    REQUIRE(decision.riskScore < 100.0);
}

TEST_CASE("RiskEngine rechaza orden que excede limite por instrumento", "[risk-engine][limits]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.maxPositionPerInstrument = 500;
    engine.updateLimits(limits);

    Order order;
    order.instrumentId = "TSLA";
    order.traderId = "TRADER_002";
    order.side = OrderSide::BUY;
    order.quantity = 1000;
    order.price = 200.00;

    MarketData marketData;
    marketData.instrumentId = "TSLA";
    marketData.bid = 199.90;
    marketData.ask = 200.10;
    marketData.last = 200.00;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == false);
    REQUIRE(decision.rejectionReason == RejectionReason::INSTRUMENT_LIMIT_EXCEEDED);
}

TEST_CASE("RiskEngine rechaza orden que excede limite por trader", "[risk-engine][limits]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.maxExposurePerTrader = 100000.0;
    engine.updateLimits(limits);

    Order order;
    order.instrumentId = "GOOGL";
    order.traderId = "TRADER_003";
    order.side = OrderSide::BUY;
    order.quantity = 2000;
    order.price = 140.00;

    MarketData marketData;
    marketData.instrumentId = "GOOGL";
    marketData.bid = 139.80;
    marketData.ask = 140.20;
    marketData.last = 140.00;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == false);
    REQUIRE(decision.rejectionReason == RejectionReason::TRADER_LIMIT_EXCEEDED);
}

TEST_CASE("RiskEngine cumple objetivo de latencia p99", "[risk-engine][latency]") {
    RiskEngine engine;
    engine.initialize();

    const int numOrders = 10000;
    std::vector<std::chrono::microseconds> latencies;
    latencies.reserve(numOrders);

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> quantityDist(1, 500);
    std::uniform_real_distribution<double> priceDist(100.0, 200.0);

    std::vector<std::string> instruments = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
    std::uniform_int_distribution<int> instrumentDist(0, instruments.size() - 1);

    for (int i = 0; i < numOrders; ++i) {
        Order order;
        order.instrumentId = instruments[instrumentDist(rng)];
        order.traderId = "TRADER_" + std::to_string(i % 10);
        order.strategyId = "STRAT_Test";
        order.side = (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL;
        order.quantity = quantityDist(rng);
        order.price = priceDist(rng);

        MarketData marketData;
        marketData.instrumentId = order.instrumentId;
        marketData.bid = order.price - 0.05;
        marketData.ask = order.price + 0.05;
        marketData.last = order.price;
        marketData.timestamp = std::chrono::system_clock::now();

        auto start = std::chrono::high_resolution_clock::now();
        auto decision = engine.evaluateOrder(order, marketData);
        auto end = std::chrono::high_resolution_clock::now();

        latencies.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }

    std::sort(latencies.begin(), latencies.end());
    auto p99Index = static_cast<size_t>(numOrders * 0.99);
    auto p99Latency = latencies[p99Index];

    REQUIRE(p99Latency.count() < 500);
}

TEST_CASE("RiskEngine mantiene consistencia entre evaluaciones concurrentes", "[risk-engine][concurrency]") {
    RiskEngine engine;
    engine.initialize();

    const int numThreads = 4;
    const int ordersPerThread = 1000;
    std::atomic<int> approvedCount{0};
    std::atomic<int> rejectedCount{0};

    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&engine, t, ordersPerThread, &approvedCount, &rejectedCount]() {
            std::mt19937 rng(t * 1000);
            std::uniform_int_distribution<int> qtyDist(1, 100);

            for (int i = 0; i < ordersPerThread; ++i) {
                Order order;
                order.instrumentId = "AAPL";
                order.traderId = "TRADER_" + std::to_string(t);
                order.strategyId = "STRAT_Concurrent";
                order.side = OrderSide::BUY;
                order.quantity = qtyDist(rng);
                order.price = 150.00;

                MarketData marketData;
                marketData.instrumentId = "AAPL";
                marketData.bid = 149.95;
                marketData.ask = 150.05;
                marketData.last = 150.00;
                marketData.timestamp = std::chrono::system_clock::now();

                auto decision = engine.evaluateOrder(order, marketData);

                if (decision.approved) {
                    approvedCount++;
                } else {
                    rejectedCount++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int total = approvedCount + rejectedCount;
    REQUIRE(total == numThreads * ordersPerThread);
    REQUIRE(approvedCount > 0);
}

TEST_CASE("RiskEngine calcula VaR intraday correctamente", "[risk-engine][var]") {
    RiskEngine engine;
    engine.initialize();

    std::vector<MarketData> priceHistory;
    std::mt19937 rng(123);
    std::normal_distribution<double> returnsDist(0.0, 0.02);

    double lastPrice = 100.0;
    for (int i = 0; i < 252; ++i) {
        lastPrice *= (1.0 + returnsDist(rng));
        MarketData md;
        md.instrumentId = "SPY";
        md.last = lastPrice;
        md.timestamp = std::chrono::system_clock::now() - std::chrono::hours(252 - i);
        priceHistory.push_back(md);
    }

    engine.updatePriceHistory("SPY", priceHistory);

    auto varResult = engine.calculateVaR("SPY", 0.95, 1);

    REQUIRE(varResult.value > 0.0);
    REQUIRE(varResult.confidenceLevel == 0.95);
}

TEST_CASE("RiskEngine responde a cambios dinamicos de volatilidad", "[risk-engine][volatility]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.volatilityMultiplier = 1.5;
    engine.updateLimits(limits);

    std::vector<MarketData> lowVolHistory;
    double price = 100.0;
    for (int i = 0; i < 20; ++i) {
        price *= (1.0 + ((double)rand() / RAND_MAX - 0.5) * 0.01);
        MarketData md;
        md.instrumentId = "TEST";
        md.last = price;
        md.timestamp = std::chrono::system_clock::now();
        lowVolHistory.push_back(md);
    }
    engine.updatePriceHistory("TEST", lowVolHistory);

    Order lowVolOrder;
    lowVolOrder.instrumentId = "TEST";
    lowVolOrder.traderId = "TRADER_VOL";
    lowVolOrder.quantity = 100;
    lowVolOrder.price = price;
    lowVolOrder.side = OrderSide::BUY;

    MarketData lowVolMd;
    lowVolMd.instrumentId = "TEST";
    lowVolMd.last = price;
    lowVolMd.timestamp = std::chrono::system_clock::now();

    auto lowVolDecision = engine.evaluateOrder(lowVolOrder, lowVolMd);

    std::vector<MarketData> highVolHistory;
    price = 100.0;
    for (int i = 0; i < 20; ++i) {
        price *= (1.0 + ((double)rand() / RAND_MAX - 0.5) * 0.05);
        MarketData md;
        md.instrumentId = "TEST";
        md.last = price;
        md.timestamp = std::chrono::system_clock::now();
        highVolHistory.push_back(md);
    }
    engine.updatePriceHistory("TEST", highVolHistory);

    auto highVolDecision = engine.evaluateOrder(lowVolOrder, lowVolMd);

    REQUIRE(highVolDecision.riskScore >= lowVolDecision.riskScore);
}