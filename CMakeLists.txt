cmake_minimum_required(VERSION 3.25)
project(arbitrage_bot)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_executable(arbitrage_bot
        src/main.cpp
        src/Graph.cpp
        src/MarketDataProvider.cpp
        src/Logger.cpp
        include/OrderManager.h
        src/OrderManager.cpp
)
target_include_directories(arbitrage_bot PRIVATE include)

target_link_libraries(arbitrage_bot PRIVATE
        cpr::cpr
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)
enable_testing()
include(GoogleTest)
add_executable(run_tests tests/GraphTests.cpp src/Graph.cpp)
target_include_directories(run_tests PRIVATE include )
target_link_libraries(run_tests PRIVATE GTest::gtest_main)
gtest_discover_tests(run_tests)

add_custom_command(TARGET arbitrage_bot POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.json"
        "$<TARGET_FILE_DIR:arbitrage_bot>/config.json"
)