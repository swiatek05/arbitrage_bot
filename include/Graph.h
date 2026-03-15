//
// Created by Michał on 14.03.2026.
//

#ifndef ARBITRAGE_BOT_GRAPH_H
#define ARBITRAGE_BOT_GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

struct Edge {
    int source; //crypto which I have now
    int destination; //crypto which I want buy
    double weight;
};
const double MIN_PROFIT_PERCENT=0.05;

class Graph {
private:
    int count_of_nodes; //count of crypto
    std::vector<Edge> edges;
    std::vector<std::string> currencyNames; //text information for us
    std::unordered_map<std::string,int>indexNames;
    std::mutex graph_mutex;

public:
    std:: vector<std::string> last_detected_path;
    Graph(int count_of_nodes);
    int getIdOrAddNode(const std:: string& name);
    void setCurrencyName(int index,const std::string& name);
    void addEdge(const std::string source, const std::string destination, double rate);
    void addEdgeById(int source_id, int destination_id, double rate);
    void clearEdges();
    bool bellmanFord();
    void lock(){graph_mutex.lock();}
    void unlock(){graph_mutex.unlock();}
};

#endif //ARBITRAGE_BOT_GRAPH_H
