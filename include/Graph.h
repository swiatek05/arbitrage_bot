//
// Created by Michał on 14.03.2026.
//

#ifndef ARBITRAGE_BOT_GRAPH_H
#define ARBITRAGE_BOT_GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>

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
    std::unordered_map<std::string,int>indexNames;

public:
    std:: vector<std::string> last_detected_path;
    Graph(int count_of_nodes);
    int getIdOrAddNode(const std:: string& name);
    void setCurrencyName(int index,const std::string& name);
    void addEdge(const std::string source, const std::string destination, double rate);
    bool bellmanFord();
};

#endif //ARBITRAGE_BOT_GRAPH_H
