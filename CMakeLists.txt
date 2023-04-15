cmake_minimum_required(VERSION 3.20)
project(klblog)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(fmt)

add_subdirectory(src)
