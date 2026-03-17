cmake_minimum_required(VERSION 4.1)
project(arbitrage_bot)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include_directories(include)

add_executable(arbitrage_bot src/main.cpp
        include/Graph.h
        src/Graph.cpp
        include/MarketDataProvider.h
        src/MarketDataProvider.cpp
        src/Logger.cpp
        include/Logger.h
)
add_executable(run_tests
        tests/GraphTests.cpp
        src/Graph.cpp
)
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
FetchContent_Declare(json URL https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz)
FetchContent_Declare(cpr URL https://github.com/libcpr/cpr/archive/refs/tags/1.14.2.tar.gz)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(CPR_CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest json cpr)
target_link_libraries(run_tests
        GTest::gtest_main
)

target_link_libraries(arbitrage_bot PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

enable_testing()
include(GoogleTest)
gtest_discover_tests(run_tests)

