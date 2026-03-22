//
// Created by Michał on 17.03.2026.
//

#include "Logger.h"
#include <iostream>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <format>

full_and_short_date Logger::getCurrentTimestamp() {
    std::time_t now=std::time(nullptr);
    std::tm* local_time=std::localtime(&now);

    if (!local_time) return{"ERROR","ERROR"};

    char bufor1[30],bufor2[30];
    std::strftime(bufor1,sizeof(bufor1),"%d.%m.%Y  %H:%M:%S",local_time);
    std::strftime(bufor2,sizeof(bufor2),"%d.%m.%Y",local_time);

    std::string full_time=std::string(bufor1);
    std::string short_date=std::string(bufor2);

    return {full_time,short_date} ;
}

void Logger::check_and_add_header(const std::string &filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::ofstream file(filepath);
        file<<"Timestamp;Expected_Profit_Percent;Path_Details;Real_Profit_Percent;Real_Profit;Available_USDC\n";
        file.close();
    }
}

void Logger::add_log(double expected_profit_percent,const std::string &path_details,double real_profit_percent,double real_profit,double available_usdc)
{
    std::lock_guard<std::mutex> quard(log_mutex);
    full_and_short_date dates=getCurrentTimestamp();

    std::string path_general="/arbitrage_bot/logs/logs_all.csv";
    std::string path_today="/arbitrage_bot/logs/logs_"+dates.short_date+".csv";

    check_and_add_header(path_general);
    check_and_add_header(path_today);

    std::ofstream file_general(path_general,std::ios::app);
    std::ofstream file_today(path_today,std::ios::app);

    std::string log_line=std::format("{};{};{};{};{};{}\n",
        dates.full_date,
        expected_profit_percent,
        path_details,
        real_profit_percent,
        real_profit,
        available_usdc);
    if (file_general.is_open())
        file_general<<log_line;
    else
        std::cerr<<"Error with open file_general"<<std::endl;

    if (file_today.is_open())
        file_today<<log_line;
    else
        std::cerr<<"Error with open file_today"<<std::endl;

    file_general.close();
    file_today.close();







}
