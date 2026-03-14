//
// Created by Michał on 14.03.2026.
//
#include <gtest/gtest.h>

#include "Graph.h"

TEST(ArbitrageTest, DetectionProfitableCycle) {
    //given
    Graph market(3);
    market.setCurrencyName(0,"USD");
    market.setCurrencyName(1,"BTC");
    market.setCurrencyName(2,"ETH");
    //when
    market.addEdge(0,1,1.2);
    market.addEdge(1,2,1.5);
    market.addEdge(2,0,0.7);
    //then
    EXPECT_TRUE(market.bellmanFord());
}
TEST(ArbitrageTest,NotDetectionProfitableCycle) {
    Graph market(3);
    market.setCurrencyName(0,"USD");
    market.setCurrencyName(1,"BTC");
    market.setCurrencyName(2,"ETH");
    //when
    market.addEdge(0,1,0.1);
    market.addEdge(1,2,1.5);
    market.addEdge(2,0,0.7);
    //then
    EXPECT_FALSE(market.bellmanFord());

}