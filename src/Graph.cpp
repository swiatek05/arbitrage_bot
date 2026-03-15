//
// Created by Michał on 14.03.2026.
//

#include "Graph.h"
#include <iostream>
#include <math.h>
const double EPS=1e-9;
Graph::Graph(int count_of_nodes) {
    this->count_of_nodes =count_of_nodes;
    currencyNames.resize(count_of_nodes);
}
void Graph::setCurrencyName(int index,const std::string& name){
    if (index>=0 && index<count_of_nodes) {
        currencyNames[index]=name;
    }
    else {
        currencyNames.push_back(name);
        count_of_nodes=currencyNames.size();
    }
}
int  Graph::getIdOrAddNode(const std::string &name) {
    if (indexNames.find(name)==indexNames.end()) {
        int index=indexNames.size();
        setCurrencyName(index,name);
        indexNames[name]=index;
        return index;
    }
    return indexNames[name];
}

void Graph::addEdge(std::string source,std::string destination, double rate) {
    const double fee=0.001;
    double weight=-std::log(rate*(1-fee));
    int source_id=getIdOrAddNode(source);
    int destination_id=getIdOrAddNode(destination);
    edges.push_back({source_id,destination_id,weight});
}
void Graph::addEdgeById(int source,int destination,double rate) {
    const double fee=0.001;
    double weight=-std::log(rate*(1-fee));
    edges.push_back({source,destination,weight});
}
void Graph::clearEdges() {
    edges.clear();
    last_detected_path.clear();
}
bool Graph::bellmanFord() {
    std::vector<int> parents(count_of_nodes,-1);
    std::vector<double> distances(count_of_nodes,0); //0 cause We find negative cycle without source

    for (int i=0;i<count_of_nodes-1;i++) {
        for (const Edge& edge : edges) {
            if (distances[edge.source]+edge.weight<distances[edge.destination]-EPS) {
                distances[edge.destination]=distances[edge.source]+edge.weight;
                parents[edge.destination]=edge.source;
            }
        }
    }
    int start=-1;
    for (const Edge& edge : edges) {
        if (distances[edge.source]+edge.weight<distances[edge.destination]) {
            start=edge.destination;
            break;
        }
    }
    if (start!=-1) {
        std::cout<<"Negative cycle detected"<<std::endl;
        for (int i=0;i<count_of_nodes;i++) {
            start=parents[start];
        }
        std::vector<int> cycle;
        int current=start;
        while (true) {
            cycle.push_back(current);
            if (current==start && cycle.size()>1) {
                break;
            }
            current=parents[current];
        }

        std::cout<<"Path: "<<std::endl;
        double total_weight=0;
        last_detected_path.clear();
        for (int i=cycle.size()-1;i>=0;i--) {
            std::cout<<"a";
            std::cout<<currencyNames[cycle[i]]<<" ";
            last_detected_path.push_back(currencyNames[cycle[i]]);
            if (i>0) {
                int u=cycle[i];
                int v=cycle[i-1];
                for (const Edge& edge : edges) {
                    if (edge.source==u && edge.destination==v) {
                        total_weight+=edge.weight;
                        break;
                    }
                }
            }
        }
        std::cout<<std::endl;

        double profit_multiplied=std::exp(-total_weight);
        double profit_percent=(profit_multiplied-1.0)*100.0;
        if (profit_percent>0) {
            std::cout<<"Total profit "<<profit_percent<<" %"<<std::endl;
            if (profit_percent<MIN_PROFIT_PERCENT) {
                std::cout<<"Profit percent is too little small"<<std::endl;
                return false;
            }
            return true;
        }
        return false;

    }
    else {
        std::cout<<"Lack of negative cycle"<<std::endl;
        return false;
    }
}

