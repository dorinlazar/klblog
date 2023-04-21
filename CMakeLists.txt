cmake_minimum_required(VERSION 3.20)
project(klblog)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

find_package(fmt)

add_subdirectory(src)
add_subdirectory(test)
