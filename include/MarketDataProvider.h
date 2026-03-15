//
// Created by Michał on 15.03.2026.
//

#ifndef ARBITRAGE_BOT_MARKETDATAPROVIDER_H
#define ARBITRAGE_BOT_MARKETDATAPROVIDER_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Graph.h"
class MarketDataProvider {
private:
    std::unordered_map<std::string,std::pair<std::string,std::string>> extract_symbols;
public:
    int extractSymbolsAndReturnCount();
    void updateGraphWithMarketData(Graph &market);
};

#endif //ARBITRAGE_BOT_MARKETDATAPROVIDER_H