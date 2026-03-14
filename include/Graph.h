//
// Created by Michał on 14.03.2026.
//

#ifndef ARBITRAGE_BOT_GRAPH_H
#define ARBITRAGE_BOT_GRAPH_H

#include <vector>
#include <string>

struct Edge {
    int source; //crypto which I have now
    int destination; //crypto which I want buy
    double weight;
};

class Graph {
private:
    int count_of_nodes; //count of crypto
    std::vector<Edge> edges;
    std::vector<std::string> currencyNames; //text information for us
public:
    Graph(int count_of_nodes);
    void setCurrencyName(int index, const std::string& name);
    void addEdge(int source, int destination, double rate);
    void bellmanFord();


};

#endif //ARBITRAGE_BOT_GRAPH_H
