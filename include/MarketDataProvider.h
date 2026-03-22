//
// Created by Michał on 15.03.2026.
//

#ifndef ARBITRAGE_BOT_MARKETDATAPROVIDER_H
#define ARBITRAGE_BOT_MARKETDATAPROVIDER_H

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Graph.h"
struct PairMapping {
    std::string pairName;
    int symbol1;
    int symbol2;
    double min_notional;
    double step_size;
};
class MarketDataProvider {
private:
    std::vector<PairMapping> fast_mapping;
public:
    int extractSymbolsAndReturnCount(Graph &market);
    void updateGraphWithMarketData(Graph &market);
};

#endif //ARBITRAGE_BOT_MARKETDATAPROVIDER_H