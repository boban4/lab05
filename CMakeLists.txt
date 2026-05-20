cmake_minimum_required(VERSION 3.10)
project(lab05)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(--coverage -g -O0)
add_link_options(--coverage)

add_subdirectory(banking)
add_subdirectory(tests)
