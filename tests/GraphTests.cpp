//
// Created by Michał on 14.03.2026.
//
#include <gtest/gtest.h>
#include "Graph.h"

TEST(ArbitrageTest, DetectionProfitableCycleAndReturnCorrectPath) {
    //given
    Graph market(3);
    //when
    market.addEdge("USD","BTC",1.2);
    market.addEdge("BTC","ETH",1.5);
    market.addEdge("ETH","USD",0.7);
    //then
    std::vector<std::string> expected_path={"BTC","ETH","USD","BTC"};
    EXPECT_TRUE(market.bellmanFord());
    ASSERT_EQ(expected_path,market.last_detected_path);
}
TEST(ArbitrageTest,NotDetectionProfitableCycle) {
    Graph market(3);
    //when
    market.addEdge("USD","BTC",0.1);
    market.addEdge("BTC","ETH",1.5);
    market.addEdge("ETH","USD",0.7);
    //then
    EXPECT_FALSE(market.bellmanFord());

}