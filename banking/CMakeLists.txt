cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

project(banking)

add_library(banking STATIC Account.cpp Transaction.cpp)
target_include_directories(banking PUBLIC .)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
    
    add_executable(tests tests/test.cpp)
    target_link_libraries(tests banking GTest::gtest_main GTest::gmock_main)
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(banking PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(banking PRIVATE --coverage)
        target_compile_options(tests PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(tests PRIVATE --coverage)
    endif()
    
    include(GoogleTest)
    gtest_discover_tests(tests)

    target_compile_definitions(banking PRIVATE DEBUG)
    target_compile_definitions(tests PRIVATE DEBUG)
endif()
