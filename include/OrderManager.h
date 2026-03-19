//
// Created by Michał on 18.03.2026.
//

#ifndef ARBITRAGE_BOT_ORDERMANAGER_H
#define ARBITRAGE_BOT_ORDERMANAGER_H
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
class OrderManager {
private:
    std::string apiKey;
    std::string apiSecret;
    int recvWindow=5000;

    std::string getTimestamp();
    std::string generateSignature(std::string timestamp,std::string json_body);
public:
    OrderManager(std::string key,std::string secret)
        :apiKey(std::move(key)),apiSecret(std::move(secret)) {}

    void makeOrder(std::string symbol,std::string typ_of_transaction,double qty);
    double getWallet(const std::string &symbol_name);


};


#endif //ARBITRAGE_BOT_ORDERMANAGER_H