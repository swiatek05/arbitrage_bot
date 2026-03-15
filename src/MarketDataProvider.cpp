//
// Created by Michał on 15.03.2026.
//

#include "MarketDataProvider.h"
#include <cpr/cpr.h>
#include <set>
#include <iostream>
using json = nlohmann::json;

int MarketDataProvider::extractSymbolsAndReturnCount(Graph &market) {
    fast_mapping.clear();
    std::cout << "Checking connecion with ByBit (extract)" << std::endl;
    cpr::Response r = cpr::Get(
        cpr::Url{"https://api.bybit.com/v5/market/instruments-info"},
        cpr::Parameters{{"category", "spot"}}
    );

    if (r.status_code == 200) {
        std::set<std::string> unique_symbols;
        json json_data = json::parse(r.text);
        auto &json_symbols = json_data["result"]["list"];
        for (int i = 0; i < json_symbols.size(); i++) {
            std::string full_symbol_s = json_symbols[i]["symbol"];
            std::string symbol1_s = json_symbols[i]["baseCoin"];
            std::string symbol2_s = json_symbols[i]["quoteCoin"];

            unique_symbols.insert(symbol1_s);
            unique_symbols.insert(symbol2_s);

            int symbol1 = market.getIdOrAddNode(symbol1_s);
            int symbol2 = market.getIdOrAddNode(symbol2_s);
            fast_mapping.push_back({full_symbol_s, symbol1, symbol2});
        }

        return unique_symbols.size();
    } else {
        std::cerr << "Error extract symbols: " << r.status_code << std::endl;
    }
    return 0;
}

void MarketDataProvider::updateGraphWithMarketData(Graph &market) {
    std::cout << "Checking connecion with ByBit" << std::endl;
    cpr::Response r = cpr::Get(
        cpr::Url{"https://api.bybit.com/v5/market/tickers"},
        cpr::Parameters{{"category", "spot"}}
    );
    if (r.status_code != 200) {
        std::cerr << "Connection Error: " << r.status_code << std::endl;
        return;
    }
    try {
        json response = json::parse(r.text);
        auto &pair_list = response["result"]["list"];
        int n = pair_list.size();


        std::cout << "Download " << n << " crypto pais from Bybit" << std::endl;
        std::unordered_map<std::string, std::pair<double, double> > prices;
        for (int i = 0; i < n; i++) {
            try {
                std::string symbol = pair_list[i]["symbol"];

                std::string bid_s = pair_list[i]["bid1Price"];
                std::string ask_s = pair_list[i]["ask1Price"];

                double bid = std::stod(bid_s);
                double ask = std::stod(ask_s);

                prices[symbol] = {bid, ask};
            } catch (const std::exception &e) {
                continue;
            }
        }

        for (int j = 0; j < fast_mapping.size(); j++) {
            PairMapping mapping = fast_mapping[j];
            if (prices.find(mapping.pairName) != prices.end()) {
                auto [bid,ask] = prices[mapping.pairName];
                if (bid > 0 && ask > 0) {
                    market.addEdgeById(mapping.symbol2, mapping.symbol1, 1.0 / ask);
                    market.addEdgeById(mapping.symbol1, mapping.symbol2, bid);
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
