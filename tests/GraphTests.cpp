#include <gtest/gtest.h>
#include "Graph.h"
#include "Logger.h"

TEST(ArbitrageTest, DetectionProfitableCycleAndReturnCorrectPath) {
    // given
    double fee = 0.001;
    Graph market(3, fee);
    Logger logger;

    market.addEdge("USD", "BTC", 1.2, "BTCUSD", "BUY", 1.2, 100.0, 10.0, 0.01);
    market.addEdge("BTC", "ETH", 1.5, "ETHBTC", "BUY", 1.5, 100.0, 10.0, 0.01);
    market.addEdge("ETH", "USD", 0.7, "ETHUSD", "SELL", 0.7, 100.0, 10.0, 0.01);

    // then
    std::vector<std::string> expected_path = {"BTC", "ETH", "USD", "BTC"};
    EXPECT_TRUE(market.bellmanFord(logger));
    ASSERT_EQ(expected_path, market.last_detected_path);
}

TEST(ArbitrageTest, NotDetectionProfitableCycle) {
    // given
    double fee = 0.001;
    Graph market(3, fee);
    Logger logger;

    // when
    market.addEdge("USD", "BTC", 0.1, "BTCUSD", "BUY", 0.1, 100.0, 10.0, 0.01);
    market.addEdge("BTC", "ETH", 1.5, "ETHBTC", "BUY", 1.5, 100.0, 10.0, 0.01);
    market.addEdge("ETH", "USD", 0.7, "ETHUSD", "SELL", 0.7, 100.0, 10.0, 0.01);

    // then
    EXPECT_FALSE(market.bellmanFord(logger));
}

TEST(ArbitrageTest, ProfitDestroyedByFees) {
    // given
    double fee = 0.001;
    Graph market(3, fee);
    Logger logger;

    // when
    market.addEdge("USD", "BTC", 1.0005, "BTCUSD", "BUY", 1.0005, 100.0, 10.0, 0.01);
    market.addEdge("BTC", "ETH", 1.0005, "ETHBTC", "BUY", 1.0005, 100.0, 10.0, 0.01);
    market.addEdge("ETH", "USD", 1.0005, "ETHUSD", "SELL", 1.0005, 100.0, 10.0, 0.01);

    // then
    EXPECT_FALSE(market.bellmanFord(logger));
}

TEST(ArbitrageTest, MinProfit) {
    // given
    double fee = 0.001;
    Graph market(3, fee);
    Logger logger;

    // when
    market.addEdge("USD", "BTC", 1.001, "BTCUSD", "BUY", 1.001, 100.0, 10.0, 0.01);
    market.addEdge("BTC", "ETH", 1.001, "ETHBTC", "BUY", 1.001, 100.0, 10.0, 0.01);
    market.addEdge("ETH", "USD", 1.001, "ETHUSD", "SELL", 1.001, 100.0, 10.0, 0.01);

    // then
    EXPECT_FALSE(market.bellmanFord(logger));
}

TEST(ArbitrageTest, TwoNodeCycle) {
    // given
    double fee = 0.001;
    Graph market(2, fee);
    Logger logger;
    std::vector<std::string> expected_path1 = {"USDT", "BTC", "USDT"};
    std::vector<std::string> expected_path2 = {"BTC", "USDT", "BTC"};

    // when
    market.addEdge("USDT", "BTC", 1.0/60000.0, "BTCUSDT", "BUY", 60000.0, 10.0, 10.0, 0.0001);
    market.addEdge("BTC", "USDT", 61000.0, "BTCUSDT", "SELL", 61000.0, 10.0, 10.0, 0.0001);

    // then
    EXPECT_TRUE(market.bellmanFord(logger));
    EXPECT_TRUE(expected_path1 == market.last_detected_path || expected_path2 == market.last_detected_path);
}