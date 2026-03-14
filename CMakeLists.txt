cmake_minimum_required(VERSION 4.1)
project(arbitrage_bot)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(arbitrage_bot src/main.cpp
        include/Graph.h
        src/Graph.cpp

)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

add_executable(run_tests
        tests/GraphTests.cpp
        src/Graph.cpp
)
target_link_libraries(run_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(run_tests)
