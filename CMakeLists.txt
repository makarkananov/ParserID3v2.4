cmake_minimum_required(VERSION 3.12)

project(
        lab7
        VERSION 1.0
        DESCRIPTION "C++ YEAR 2023 Lab7"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_subdirectory(bin)