//
// Created by Michał on 14.03.2026.
//

#include "Graph.h"
#include <iostream>
#include <math.h>

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
    double weight=-std::log(rate);
    int source_id=getIdOrAddNode(source);
    int destination_id=getIdOrAddNode(destination);
    edges.push_back({source_id,destination_id,weight});
}
bool Graph::bellmanFord() {
    std::vector<int> parents(count_of_nodes,-1);
    std::vector<double> distances(count_of_nodes,0); //0 cause We find negative cycle without source

    for (int i=0;i<count_of_nodes-1;i++) {
        for (const Edge& edge : edges) {
            if (distances[edge.source]+edge.weight<distances[edge.destination]) {
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
        for (int i=cycle.size()-1;i>=0;i--) {
            std::cout<<"a";
            std::cout<<currencyNames[cycle[i]]<<" ";
            last_detected_path.push_back(currencyNames[cycle[i]]);
        }
        std::cout<<std::endl;
        return true;
    }
    else {
        std::cout<<"Lack of negative cycle"<<std::endl;
        return false;
    }
}

