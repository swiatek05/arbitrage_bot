cmake_minimum_required(VERSION 3.25)
project(arbitrage_bot)

set(CMAKE_CXX_STANDARD 20)

if(WIN32 AND NOT DEFINED VCPKG_TARGET_TRIPLET)
    set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
endif()


find_package(OpenSSL REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

include_directories(include)
add_executable(arbitrage_bot
        src/main.cpp
        src/Graph.cpp
        src/MarketDataProvider.cpp
        src/Logger.cpp
)
target_link_libraries(arbitrage_bot PRIVATE
        cpr::cpr
        nlohmann_json::nlohmann_json
        OpenSSL::SSL
        OpenSSL::Crypto
)
enable_testing()
include(GoogleTest)
add_executable(run_tests tests/GraphTests.cpp src/Graph.cpp)
target_link_libraries(run_tests PRIVATE GTest::gtest_main)
gtest_discover_tests(run_tests)