cmake_minimum_required(VERSION 4.1)
project(arbitrage_bot)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(arbitrage_bot src/main.cpp
        include/Graph.h
        src/Graph.cpp
)
