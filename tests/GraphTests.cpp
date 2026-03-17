//
// Created by Michał on 14.03.2026.
//
#include <gtest/gtest.h>
#include "Graph.h"

TEST(ArbitrageTest, DetectionProfitableCycleAndReturnCorrectPath) {
    //given
    Graph market(3);
    //when
    market.addEdge("USD","BTC",1.2,"BTCUSD","KUPNO",1.2); //rate and real_rate is the same cause we want to only test bellman_ford
    market.addEdge("BTC","ETH",1.5,"ETHBTC","KUPNO",1.5);
    market.addEdge("ETH","USD",0.7,"ETHUSD","SPRZEDAZ",0.7);
    //then
    std::vector<std::string> expected_path={"BTC","ETH","USD","BTC"};
    EXPECT_TRUE(market.bellmanFord());
    ASSERT_EQ(expected_path,market.last_detected_path);
}
TEST(ArbitrageTest,NotDetectionProfitableCycle) {
    //given
    Graph market(3);
    //when
    market.addEdge("USD","BTC",0.1,"BTCUSD","KUPNO",0.1);
    market.addEdge("BTC","ETH",1.5,"ETHBTC","KUPNO",1.5);
    market.addEdge("ETH","USD",0.7,"ETHUSD","SPRZEDAZ",0.7);
    //then
    EXPECT_FALSE(market.bellmanFord());

}
TEST(ArbitrageTest,ProtitDestroyedByFees) {
    //given
    Graph market(3);
    //when
    market.addEdge("USD", "BTC", 1.0005,"BTCUSD","KUPNO",1.0005);
    market.addEdge("BTC", "ETH", 1.0005,"ETHBTC","KUPNO",1.0005);
    market.addEdge("ETH", "USD", 1.0005,"ETHUSD","SPRZEDAZ",1.0005);
    //then
    EXPECT_FALSE(market.bellmanFord());
}
TEST(ArbitrageTest,MinProfit) {
    //given
    Graph market(3);
    //when
    market.addEdge("USD", "BTC", 1.001,"BTCUSD","KUPNO",1.001);
    market.addEdge("BTC", "ETH", 1.001,"ETHBTC","KUPNO",1.001);
    market.addEdge("ETH", "USD", 1.001,"ETHUSD","SPRZEDAZ",1.001);
    //then
    EXPECT_FALSE(market.bellmanFord());
}
TEST(ArbitrageTest,TwoNodeCycle) {
    //given
    Graph market(2);
    std::vector<std::string> expected_path1={"USDT","BTC","USDT"};
    std::vector<std::string> expected_path2={"BTC","USDT","BTC"};

    //when
    market.addEdge("USDT", "BTC", 60000.0,"BTCUSDT","KUPNO",60000.0); //here rate is different cause We want to count on the real prices
    market.addEdge("BTC", "USDT", 61000.0,"BTCUSDT","KUPNO",61000.0);
    //then
    EXPECT_TRUE(market.bellmanFord());
    EXPECT_TRUE(expected_path1==market.last_detected_path || expected_path2==market.last_detected_path);

}