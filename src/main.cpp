#include <iostream>
#include "Graph.h"
#include "MarketDataProvider.h"
#include <thread>
#include <chrono>

int main() {
    MarketDataProvider data;

    int count_of_nodes=data.extractSymbolsAndReturnCount();
    if (count_of_nodes==0) {
        std::cerr<<"No nodes found"<<std::endl;
        return 1;
    }
    std::cout<<"Init market for "<<count_of_nodes<<" unique pair"<<std::endl;
    while (true) {
        try {
            Graph market(count_of_nodes);
            data.updateGraphWithMarketData(market);
            if (market.bellmanFord()) {
                std::cout<<"Find good arbitrage"<<std::endl;

            }
            else {
                std::cout<<"NO good arbitrage"<<std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }catch(std::exception& e) {
            std::cerr<<"error in while "<<e.what()<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    return 0;
}