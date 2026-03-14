//
// Created by Michał on 14.03.2026.
//

#include "Graph.h"
#include <iostream>
#include <math.h>
#include <cfloat>

Graph::Graph(int count_of_nodes) {
    this->count_of_nodes =count_of_nodes;
    currencyNames.resize(count_of_nodes);
}
void Graph::setCurrencyName(int index,const std::string& name){
    if (index>=0 && index<count_of_nodes) {
        currencyNames[index]=name;
    }
}

void Graph::addEdge(int source, int destination, double rate) {
    double weight=-std::log(rate);
    edges.push_back({source,destination,weight});
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
            std::cout<<currencyNames[cycle[i]]<<" ";
        }
        std::cout<<std::endl;
        return true;
    }
    else {
        std::cout<<"Lack of negative cycle"<<std::endl;
        return false;
    }
}

