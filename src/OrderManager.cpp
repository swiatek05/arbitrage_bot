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
#include <locale>
#include <cmath>
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
double OrderManager::makeOrder(std::string symbol,std::string typ_of_transaction,double qty,double step_size) {
    if (std::isnan(qty) || std::isinf(qty) || qty<=0) {
        std::cerr<<"Error with quantity (nan/inf/0)"<<std::endl;
        return 0.0;
    }
    if (step_size<=0) {
        std::cerr<<"step_size is 0.0"<<std::endl;
        return 0.0;
    }
    double safe_qty=qty*0.9999;
    double rounded_qty=std::floor(safe_qty/step_size)*step_size;
    if (std::isnan(rounded_qty) || rounded_qty<=0) {
        std::cerr<<"Error with rounded_qty="<<rounded_qty<<std::endl;
        return 0.0;
    }
    int precision=0;
    if (step_size<1.0) {
        precision=static_cast<int>(std::round(-std::log10(step_size)));
    }
    if (precision<0) precision=0;
    if (precision>20) precision=20;

    std::stringstream ss;
    ss.imbue(std::locale::classic());  //for dot
    ss<<std::fixed<<std::setprecision(precision)<<rounded_qty;
    std::string qty_str=ss.str();

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
        std::cout<<"Order completed: "<<side<<" "<<qty_str<<" "<<symbol<<std::endl;
        return rounded_qty;
    }
    else {
        std::cerr<<"Error Binance: "<<response.text<<" Code: "<<response.status_code<<" Data: "<<qty_str<<std::endl;
        return 0.0;
    }

}
double OrderManager::getWallet(const std::string &crypto_name) {
    //std::cout << "DEBUG KLUCZA: [" << apiKey << "] Dlugosc: " << apiKey.length() << std::endl;
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
        try {
            json res=json::parse(response.text);
            auto &balances=res["balances"];
            for (auto &item:balances) {
                if (item["asset"]==crypto_name) {
                    std::string crypto=item["free"];
                    return std::stod(crypto);
                }
            }
            return 0.0;
        }catch (const std::exception &e) {
            std::cerr<<"Error parsing wallet JSON"<<e.what()<<std::endl;
            return 0.0;
        }

    }
    std::cerr<<"Error with wallet download: "<<response.status_code<<" - "<<response.text<<std::endl;
    return 0.0;


}