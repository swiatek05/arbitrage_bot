//
// Created by Michał on 15.03.2026.
//

#include "MarketDataProvider.h"
#include <cpr/cpr.h>
#include <set>
#include <iostream>
using json = nlohmann::json;
struct TickerInfo {
    double bid;
    double ask;
    double bid_qty;
    double ask_qty;
};

int MarketDataProvider::extractSymbolsAndReturnCount(Graph &market) {
    fast_mapping.clear();
    //std::cout << "Checking connecion with Binance (extract)" << std::endl;
    cpr::Response r = cpr::Get(
        cpr::Url{"https://testnet.binance.vision/api/v3/exchangeInfo"}

    );


    if (r.status_code == 200) {
        std::set<std::string> unique_symbols;
        try {
            json json_data = json::parse(r.text);
            auto &json_symbols = json_data["symbols"];
            for (int i = 0; i < json_symbols.size(); i++) {
                if (json_symbols[i]["status"]!="TRADING") continue;
                std::string full_symbol_s = json_symbols[i]["symbol"];
                std::string symbol1_s = json_symbols[i]["baseAsset"];
                std::string symbol2_s = json_symbols[i]["quoteAsset"];

                double min_notional=10.0;
                double step_size=1.0;

                for (auto &filter : json_symbols[i]["filters"]) {
                    if (filter["filterType"] == "NOTIONAL" || filter["filterType"] == "MIN_NOTIONAL") {
                        std::string min_not = filter["minNotional"];
                        min_notional = std::stod(min_not);
                    }
                    if (filter["filterType"] == "LOT_SIZE") {
                        std::string s_size = filter["stepSize"];
                        step_size = std::stod(s_size);
                    }
                }
                for (auto &filter: json_symbols[i]["filters"]) {
                    if (filter["filterType"]=="MARKET_LOT_SIZE") {
                        std::string s_size=filter["stepSize"];
                        double market_step_size=std::stod(s_size);

                        if (market_step_size>0.0) {
                            step_size=market_step_size;
                        }
                    }
                }


                if (step_size <= 0) {
                    continue;
                }




                //std::cout<<full_symbol_s<<" "<<step_size<<std::endl;



                unique_symbols.insert(symbol1_s);
                unique_symbols.insert(symbol2_s);

                int symbol1 = market.getIdOrAddNode(symbol1_s);
                int symbol2 = market.getIdOrAddNode(symbol2_s);
                fast_mapping.push_back({full_symbol_s, symbol1, symbol2,min_notional,step_size});
            }

            return unique_symbols.size();
        }catch (const std::exception &e) {
            std::cerr<<"JSON Parse Error in extractSymbols"<<e.what()<<std::endl;
            return 0;
        }
    } else {
        std::cerr << "Error extract symbols: " << r.status_code << std::endl;
    }
    return 0;
}

void MarketDataProvider::updateGraphWithMarketData(Graph &market) {
    //std::cout << "Checking connecion with Binance" << std::endl;
    cpr::Response r = cpr::Get(
        cpr::Url{"https://testnet.binance.vision/api/v3/ticker/bookTicker"}

    );
    if (r.status_code != 200) {
        std::cerr << "Connection Error: " << r.status_code << std::endl;
        return;
    }
    try {
        json pair_list = json::parse(r.text);

        int n = pair_list.size();


        //std::cout << "Download " << n << " crypto pais from Bybit" << std::endl;
        std::unordered_map<std::string, TickerInfo > prices;
        for (int i = 0; i < n; i++) {
            try {
                std::string symbol = pair_list[i]["symbol"];

                std::string bid_s = pair_list[i]["bidPrice"];
                std::string ask_s = pair_list[i]["askPrice"];
                std::string bid_qty_s= pair_list[i]["bidQty"];
                std::string ask_qty_s= pair_list[i]["askQty"];

                double bid = std::stod(bid_s);
                double ask = std::stod(ask_s);
                double bid_qty = std::stod(bid_qty_s);
                double ask_qty = std::stod(ask_qty_s);

                prices[symbol] = {bid, ask,bid_qty,ask_qty};
            } catch (const std::exception &e) {
                continue;
            }
        }
        std::lock_guard<std::mutex> lock(market.getMutex());
        market.clearEdges();
        for (int j = 0; j < fast_mapping.size(); j++) {
            PairMapping mapping = fast_mapping[j];
            if (prices.find(mapping.pairName) != prices.end()) {
                auto [bid,ask,bid_qty,ask_qty] = prices[mapping.pairName];
                if (bid > 0 && ask > 0) {
                    market.addEdgeById(mapping.symbol2, mapping.symbol1, (1.0 / ask),mapping.pairName,"KUPNO",ask,ask_qty,mapping.min_notional,mapping.step_size);
                    market.addEdgeById(mapping.symbol1, mapping.symbol2, bid,mapping.pairName,"SPRZEDAZ",bid,bid_qty,mapping.min_notional,mapping.step_size);
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
