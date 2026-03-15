#include <iostream>
#include "Graph.h"
#include "MarketDataProvider.h"
#include <thread>
#include <chrono>
void download_data_thread(MarketDataProvider& data,Graph& market) {
    while (true) {
        try {
            market.lock();
            market.clearEdges();
            data.updateGraphWithMarketData(market);
            market.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

        }catch(std::exception& e) {
            market.unlock();
            std::cerr<<"Data download error "<<e.what()<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}
void bellman_ford_thread(Graph &market) {
    while (true) {
        market.lock();
        bool found=market.bellmanFord();
        market.unlock();
        if (found) {
            std::cout<<"Found profitable arbitrage"<<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}
int main() {
    MarketDataProvider data;
    Graph market(0);

    int count_of_nodes=data.extractSymbolsAndReturnCount(market);
    if (count_of_nodes==0) {
        std::cerr<<"No nodes found"<<std::endl;
        return 1;
    }
    std::cout<<"Init market for "<<count_of_nodes<<" unique pair"<<std::endl;

    std::thread t1(download_data_thread,std::ref(data),std::ref(market));
    std::thread t2(bellman_ford_thread,std::ref(market));

    t1.join();
    t2.join();

    return 0;
}