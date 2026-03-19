//
// Created by Michał on 18.03.2026.
//

#include "OrderManager.h"
#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <openssl/hmac.h>
using json = nlohmann::json;
std::string OrderManager::getTimestamp() {
    auto now=std::chrono::system_clock::now();
    auto  duration=now.time_since_epoch();
    auto millisec=std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::to_string(millisec);

}
std::string OrderManager::generateSignature(std::string timestamp,std::string  json_body) {
    std::string signature=timestamp+apiKey+std::to_string(recvWindow)+json_body;
    unsigned char* hash=HMAC(EVP_sha256(),apiSecret.c_str(),static_cast<int>(apiSecret.length()),
        reinterpret_cast<const unsigned char*>(signature.c_str()),signature.length(),NULL,NULL);
    std::stringstream ss;
    for (int i=0;i<32;i++) {
        ss<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<int>(hash[i]);
    }
    return ss.str();


}
void OrderManager::makeOrder(std::string symbol,std::string typ_of_transaction,double qty) {
    json body;
    body["category"]="spot";
    body["symbol"]=symbol;
    body["side"]=typ_of_transaction;
    body["orderType"]="Market";
    body["qty"]=std::to_string(qty);
    std::string json_body=body.dump();

    std::string timestamp=getTimestamp();

    std:: string signature=generateSignature(timestamp,json_body);

    std::string url = "https://api-testnet.bybit.com/v5/order/create";

    auto response=cpr::Post(
        cpr::Url{url},
        cpr::Header{
                {"X-BAPI-API-KEY", apiKey},
                {"X-BAPI-SIGN", signature},
                {"X-BAPI-TIMESTAMP", timestamp},
                {"X-BAPI-RECV-WINDOW", std::to_string(recvWindow)},
                {"Content-Type", "application/json"}
        },
        cpr::Body{json_body});

    if (response.status_code==200) {
        auto res_json=json::parse(response.text);
        if (res_json["retCode"]==0) {
            std::cout<<"Order completed"<<std::endl;
        }
        else {
            std::cerr<<"Error ByBit"<<res_json["retMsg"]<<" Code: "<<res_json["retCode"]<<" )"<<std::endl;
        }
    }
    else {
        std::cerr<<"Error connected "<<response.status_code<<" "<<response.error.message<<std::endl;
    }

}
double OrderManager::getWallet(const std::string &crypto_name) {
    std::string query="accountType=UNIFIED&coin="+crypto_name;
    std::string timestamp=getTimestamp();
    std::string signature=generateSignature(timestamp,query);
    std::string url = "https://api-testnet.bybit.com/v5/account/wallet-balance?" + query;

    auto response=cpr::Get(
        cpr::Url{url},
        cpr::Header{
                {"X-BAPI-API-KEY", apiKey},
                {"X-BAPI-SIGN", signature},
                {"X-BAPI-TIMESTAMP", timestamp},
                {"X-BAPI-RECV-WINDOW", std::to_string(recvWindow)}
        });

    if (response.status_code==200) {
        json res=json::parse(response.text);
        if (res["retCode"]==0) {
            auto &cryptos=res["result"]["list"][0]["coin"];
            for (auto &item:cryptos) {
                if (item["coin"]==crypto_name) {
                    std::string crypto=item["walletBalance"];
                    return std::stod(crypto);
                }
            }
        }
    }
    std::cerr<<"Error with wallet download"<<std::endl;
    return 0.0;


}