#include <iostream>
#include "Graph.h"
#include "MarketDataProvider.h"
#include "Logger.h"
#include "OrderManager.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <fstream>
double calculateMaxSafeInput(const std::vector<Edge> &cycle_edges,double starting_balance) {
    double max_input=starting_balance;
    double current_multipier=1.0;
    const double fee=0.001;

    for (const Edge& edge : cycle_edges) {
        double step_fee_factor=(1.0-fee);
        double edge_limit_at_start=0.0;

        if (edge.typ_of_transaction=="KUPNO") {
            edge_limit_at_start=(edge.real_rate*edge.available_qty)/(current_multipier*step_fee_factor);
        }
        else {
            edge_limit_at_start=edge.available_qty/current_multipier;
        }
        max_input=std::min(max_input,edge_limit_at_start);
        current_multipier*=(edge.real_rate*step_fee_factor);
    }
    return max_input*0.9; //for safe
}

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
void bellman_ford_thread(Graph &market,Logger &logger,OrderManager &om) {
    while (true) {
        market.lock();
        bool found=market.bellmanFord(logger);
        std::vector<Edge> trade_cycle;
        if (found) {
            trade_cycle=market.last_cycle_edge; //copy cause different thread can overwrite
        }
        market.unlock();
        if (found && !trade_cycle.empty()) {
            int target_start_id=market.getIdOrAddNode("USDT");
            auto it=std::find_if(trade_cycle.begin(),trade_cycle.end(),[&](const Edge& e) {
                return e.source==target_start_id;
            });
            if (it!=trade_cycle.end()) {
                std::rotate(trade_cycle.begin(),it,trade_cycle.end());
                std::string start_coin="USDT";




                std::cout<<"Found profitable arbitrage"<<std::endl;

                double my_balance=om.getWallet("USDT");
                std::cout<<"Available balance: "<<my_balance<<std::endl;

                double safe_trade_amount=calculateMaxSafeInput(trade_cycle,my_balance);
                std::cout<<"Max safe input amount: "<<safe_trade_amount<<std::endl;

                if (safe_trade_amount<5.0) {
                    std::cout<<"Too little to trade"<<std::endl;
                }
                else {
                    double current_amount=safe_trade_amount;
                    const double fee=0.001;
                    for (const Edge &edge: trade_cycle) {
                        double order_qty=0.0;
                        if (edge.typ_of_transaction=="KUPNO") {
                            order_qty=current_amount/edge.real_rate;
                            om.makeOrder(edge.symbol_name,"Buy",order_qty);
                            current_amount=order_qty*(1.0-fee);
                        }
                        else {
                            order_qty=current_amount;
                            om.makeOrder(edge.symbol_name,"Sell",order_qty);
                            current_amount=order_qty*edge.real_rate*(1.0-fee);
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    }
                    std::cout<<"Cycle finished. Expected result: "<<current_amount<<std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
            }

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}
int main() {
    Logger logger;
    std::ifstream config_file("config.json");
    if (!config_file.is_open()) {
        std::cerr<<"Could not open config file"<<std::endl;
        return 1;
    }
    json config;
    config_file>>config;
    std::string api_key=config["api_key"];
    std::string api_secret=config["api_secret"];
    OrderManager om(api_key,api_secret);


    MarketDataProvider data;
    Graph market(0);

    int count_of_nodes=data.extractSymbolsAndReturnCount(market);
    if (count_of_nodes==0) {
        std::cerr<<"No nodes found"<<std::endl;
        return 1;
    }
    std::cout<<"Init market for "<<count_of_nodes<<" unique pair"<<std::endl;

    std::thread t1(download_data_thread,std::ref(data),std::ref(market));
    std::thread t2(bellman_ford_thread,std::ref(market),std::ref(logger),std::ref(om));

    t1.join();
    t2.join();

    return 0;
}