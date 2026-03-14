#include <iostream>
#include <Graph.h>
int main() {
    Graph market(3);
    market.setCurrencyName(0,"USD");
    market.setCurrencyName(1,"BTC");
    market.setCurrencyName(2,"ETH");

    market.addEdge(0,1,1.2);
    market.addEdge(1,2,1.5);
    market.addEdge(2,0,0.7);

    market.bellmanFord();
    return 0;
}