#ifndef ARBITRAGE_BOT_LOGGER_H
#define ARBITRAGE_BOT_LOGGER_H
#include <mutex>
#include <string>
struct full_and_short_date {
    std::string full_date;
    std::string short_date;

};
class Logger {
private:
    std::mutex log_mutex;
    static full_and_short_date getCurrentTimestamp();
    void check_and_add_header(const std::string &filepath);
public:
    Logger()=default;
    void add_log(double expected_profit_percent,const std::string &path_details,double real_profit_percent,double real_profit,double available_usdt );
    void lock(){log_mutex.lock();}
    void unlock(){log_mutex.unlock();}
};

#endif //ARBITRAGE_BOT_LOGGER_H