//
// Created by Michał on 14.03.2026.
//

#ifndef ARBITRAGE_BOT_GRAPH_H
#define ARBITRAGE_BOT_GRAPH_H

#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include "Logger.h"
#include <openssl/hmac.h>

struct Edge {
    int source; //crypto which I have now
    int destination; //crypto which I want buy
    double weight;

    std:: string symbol_name;
    std::string  typ_of_transaction; //sell or buy
    double real_rate;
    double available_qty;
    double min_notional;
    double step_size;
};
const double MIN_PROFIT_PERCENT=0.03;

class Graph {
private:
    int count_of_nodes; //count of crypto
    std::vector<Edge> edges;
    std::vector<std::string> currencyNames; //text information for us
    std::unordered_map<std::string,int>indexNames;
    std::mutex graph_mutex;

public:
    std:: vector<std::string> last_detected_path;
    std:: vector<Edge> last_cycle_edge;
    Graph(int count_of_nodes);
    int getIdOrAddNode(const std:: string& name);
    void setCurrencyName(int index,const std::string& name);
    void addEdge(const std::string source, const std::string destination, double rate,const std::string &symbol_name,const std::string &typ_of_transaction,double real_rate,double qty,double min_notional,double step_size);
    void addEdgeById(int source_id, int destination_id, double rate,const std::string &symbol_name,const std::string &typ_of_transaction,double real_rate,double qty,double min_notional,double step_size);
    void clearEdges();
    bool bellmanFord(Logger &logger);
    std::mutex &getMutex() {return graph_mutex;}
};

#endif //ARBITRAGE_BOT_GRAPH_H
