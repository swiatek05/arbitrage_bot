//
// Created by Michał on 15.03.2026.
//

#include "MarketDataProvider.h"
#include <cpr/cpr.h>
#include <set>
#include <iostream>
using json=nlohmann::json;
int MarketDataProvider::extractSymbolsAndReturnCount() {
    std::cout<<"Checking connecion with ByBit (extract)"<<std::endl;
    cpr::Response r=cpr::Get(
        cpr::Url{"https://api.bybit.com/v5/market/instruments-info"},
        cpr::Parameters{{"category","spot"}}
        );

    if (r.status_code == 200) {
        std::set<std::string>unique_symbols;
        json json_data=json::parse(r.text);
        auto& json_symbols=json_data["result"]["list"];
        for (int i=0;i<json_symbols.size();i++) {
            std::string full_symbol=json_symbols[i]["symbol"];
            std::string symbol1=json_symbols[i]["baseCoin"];
            std::string symbol2=json_symbols[i]["quoteCoin"];
            unique_symbols.insert(symbol1);
            unique_symbols.insert(symbol2);
            extract_symbols[full_symbol]={symbol1,symbol2};
        }
        std::cout<<"Positively extracted symbols: "<<extract_symbols.size()<<std::endl;
        return unique_symbols.size();

    }
    else {
        std::cerr<<"Error extract symbols: "<<r.status_code<<std::endl;
    }
    return 0;
}

void MarketDataProvider::updateGraphWithMarketData(Graph &market) {
    std::cout<<"Checking connecion with ByBit"<<std::endl;
    cpr::Response r=cpr::Get(
        cpr::Url{"https://api.bybit.com/v5/market/tickers"},
        cpr::Parameters{{"category","spot"}}
        );
    if (r.status_code != 200) {
        std::cerr<<"Connection Error: "<<r.status_code<<std::endl;
        return;
    }
    try {
        json response =json::parse(r.text);
        auto& pair_list=response["result"]["list"];
        int n=pair_list.size();



        std::cout<<"Download "<<n<<" crypto pais from Bybit"<<std::endl;
        for (int i=0;i<n;i++) {
            std::string symbol = pair_list[i]["symbol"];
            if (extract_symbols.find(symbol)!=extract_symbols.end()) {
                try {
                    std::string symbol1=extract_symbols[symbol].first;
                    std::string symbol2=extract_symbols[symbol].second;

                    std::string bid_s=pair_list[i]["bid"];
                    std::string ask_s=pair_list[i]["ask"];

                    double bid=std::stod(bid_s);
                    double ask=std::stod(ask_s);

                    if (bid<=0 || ask<=0) {
                        continue;
                    }
                    market.addEdge(symbol2,symbol1,1.0/ask);
                    market.addEdge(symbol1,symbol2,bid);
                }catch (const std::exception &e) {
                    continue;
                }
            }

        }
    }catch (const std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }

}
