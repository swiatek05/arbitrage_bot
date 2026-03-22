#include <iostream>
#include "Graph.h"
#include "MarketDataProvider.h"
#include "Logger.h"
#include "OrderManager.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <clocale>
double calculateMaxSafeInput(const std::vector<Edge> &cycle_edges,double starting_balance) {
    if (starting_balance<=0) return 0;
    double max_input=starting_balance;
    double current_multipier=1.0;
    const double fee=0.001;

    for (const Edge& edge : cycle_edges) {
        double step_fee_factor=(1.0-fee);
        if (current_multipier<=0 || step_fee_factor<=0) return 0;
        double edge_limit_at_start=0.0;

        if (edge.typ_of_transaction=="KUPNO") {
            if (edge.real_rate<=0) return 0;
            edge_limit_at_start=(edge.real_rate*edge.available_qty)/(current_multipier);
            current_multipier=(current_multipier/edge.real_rate)*step_fee_factor;
        }
        else {
            edge_limit_at_start=edge.available_qty/current_multipier;
            current_multipier=(current_multipier*edge.real_rate)*step_fee_factor;
        }
        max_input=std::min(max_input,edge_limit_at_start);

    }
    if (std::isnan(max_input) || std::isinf(max_input)) return 0;
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
                //std::cout<<"Available balance: "<<my_balance<<std::endl;

                double safe_trade_amount=calculateMaxSafeInput(trade_cycle,my_balance);
                //std::cout<<"Max safe input amount: "<<safe_trade_amount<<std::endl;

                bool fails_notional=false;
                double simulation_amount=safe_trade_amount;

                const double fee=0.001;
                if (std::isnan(simulation_amount) || std::isinf(simulation_amount) || simulation_amount <= 0) {
                    fails_notional = true;
                } else {
                    for (const Edge &edge: trade_cycle) {
                        double order_qty=0.0;
                        if (edge.typ_of_transaction=="KUPNO") {
                            order_qty=simulation_amount/edge.real_rate;
                        }
                        else {
                            order_qty=simulation_amount;
                        }
                        double safe_qty=order_qty*0.9999;
                        double rounded_qty=std::floor(safe_qty/edge.step_size)*edge.step_size;
                        if (rounded_qty<=0.0 || std::isnan(rounded_qty)) {
                            fails_notional=true;
                            break;
                        }
                        double quote_value=rounded_qty*edge.real_rate;
                        if (quote_value<edge.min_notional) {
                            fails_notional=true;
                            break;
                        }

                        if (edge.typ_of_transaction=="KUPNO") {
                            simulation_amount=rounded_qty*(1.0-fee);
                        }
                        else {
                            simulation_amount=(rounded_qty*edge.real_rate)*(1.0-fee);
                        }
                    }
                }


                if (fails_notional) {
                    std::cout<<"Too little to trade"<<std::endl;
                }
                else {
                    std::cout<<"Detection negative cycle \n";
                    std::cout<<"Available balance"<<my_balance<<" USDT \n";
                    std::cout<<"Enter to stock exchange for "<<safe_trade_amount<<" USDT \n";
                    double current_amount=safe_trade_amount;
                    const double fee=0.001;
                    double balance_before=my_balance;
                    std::string path_details="USDT";
                    double actual_invested_usdt=0.0;
                    bool cycle_completed=true;
                    for (const Edge &edge: trade_cycle) {
                        path_details += " -> " + edge.symbol_name;
                        double order_qty=0.0;
                        double executed_qty=0.0;
                        if (edge.typ_of_transaction=="KUPNO") {
                            order_qty=current_amount/edge.real_rate;
                            executed_qty=om.makeOrder(edge.symbol_name,"Buy",order_qty,edge.step_size);
                            if (actual_invested_usdt==0.0 && executed_qty>0) {
                                actual_invested_usdt=current_amount;
                            }
                            current_amount=executed_qty*(1.0-fee);
                        }
                        else {
                            order_qty=current_amount;
                            executed_qty=om.makeOrder(edge.symbol_name,"Sell",order_qty,edge.step_size);
                            if (actual_invested_usdt==0.0 && executed_qty>0) {
                                actual_invested_usdt=current_amount;
                            }
                            current_amount=executed_qty*edge.real_rate*(1.0-fee);
                        }
                        if (executed_qty<=0.0) {
                            std::cerr<<"cycle interrupted \n";
                            cycle_completed=false;
                            break;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    }
                    if (cycle_completed && actual_invested_usdt>0.0) {
                        std::cout<<"Cycle finished. Expected result: "<<current_amount<<std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        double balance_after=om.getWallet("USDT");
                        double real_profit_usdt=balance_after-balance_before;
                        double real_profit_percent=(real_profit_usdt/actual_invested_usdt)*100.0;
                        double expected_profit_usdt = current_amount - actual_invested_usdt;
                        double expected_profit_percent = (expected_profit_usdt / actual_invested_usdt) * 100.0;

                        logger.add_log(expected_profit_percent, path_details, real_profit_percent,real_profit_usdt,balance_after);
                        std::cout<<"Real profit"<<real_profit_percent<<"\n";
                    }
                    else {
                        std::cout<<"cycle failed"<<std::endl;
                    }
                    std::cout<<"===========================================\n";

                }
            }

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}
int main() {
    std::setlocale(LC_ALL, "C");
    Logger logger;
    std::ifstream config_file("/arbitrage_bot/config.json");
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