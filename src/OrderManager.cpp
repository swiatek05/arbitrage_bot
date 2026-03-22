//
// Created by Michał on 18.03.2026.
//

#include "OrderManager.h"
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>
using json = nlohmann::json;
std::string OrderManager::getTimestamp() {
    auto now=std::chrono::system_clock::now();
    auto  duration=now.time_since_epoch();
    auto millisec=std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::to_string(millisec);

}
std::string OrderManager::generateSignature(std::string timestamp,std::string query_string) {

    unsigned char* hash=HMAC(EVP_sha256(),apiSecret.c_str(),static_cast<int>(apiSecret.length()),
        reinterpret_cast<const unsigned char*>(query_string.c_str()),query_string.length(),NULL,NULL);
    std::stringstream ss;
    for (int i=0;i<32;i++) {
        ss<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(hash[i]);
    }
    return ss.str();


}
void OrderManager::makeOrder(std::string symbol,std::string typ_of_transaction,double qty) {

    std::string qty_str;
    if (qty>=10.0){
        qty_str = std::to_string(static_cast<long long>(qty));
    } else {
        std::stringstream ss;
        ss<<std::fixed<<std::setprecision(4)<<(std::floor(qty*10000.0)/10000.0);
        qty_str=ss.str();
    }
    std::string side = (typ_of_transaction == "KUPNO" || typ_of_transaction == "Buy") ? "BUY" : "SELL";
    std::string timestamp=getTimestamp();
    std::string query_string = "symbol=" + symbol + "&side=" + side + "&type=MARKET&quantity=" + qty_str + "&timestamp=" + timestamp;

    std:: string signature=generateSignature(timestamp,query_string);

    std::string url = "https://testnet.binance.vision/api/v3/order?"+query_string+"&signature="+signature;

    auto response=cpr::Post(
        cpr::Url{url},
        cpr::Header{
                {"X-MBX-APIKEY", apiKey}

        });

    if (response.status_code==200) {
        std::cout<<"Order completed: "<<side<<" "<<qty<<" "<<symbol<<std::endl;
    }
    else {
        std::cerr<<"Error Binance: "<<response.text<<" Code: "<<response.status_code<<std::endl;
    }

}
double OrderManager::getWallet(const std::string &crypto_name) {
    std::cout << "DEBUG KLUCZA: [" << apiKey << "] Dlugosc: " << apiKey.length() << std::endl;
    std::string timestamp=getTimestamp();
    std::string query="timestamp="+timestamp;
    std::string signature=generateSignature(timestamp,query);
    std::string url = "https://testnet.binance.vision/api/v3/account?" + query+"&signature="+signature;

    auto response=cpr::Get(
        cpr::Url{url},
        cpr::Header{
                {"X-MBX-APIKEY", apiKey}

        });

    if (response.status_code==200) {
        json res=json::parse(response.text);
        auto &balances=res["balances"];
        for (auto &item:balances) {
            if (item["asset"]==crypto_name) {
                std::string crypto=item["free"];
                return std::stod(crypto);
            }
        }

    }
    std::cerr<<"Error with wallet download: "<<response.status_code<<" - "<<response.text<<std::endl;
    return 0.0;


}