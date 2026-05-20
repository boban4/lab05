cmake_minimum_required(VERSION 3.10)
project(lab05)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(tests test.cpp)

target_link_libraries(tests PRIVATE banking gtest gtest_main gmock)

add_test(NAME banking_tests COMMAND tests)
