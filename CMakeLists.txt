cmake_minimum_required(VERSION 3.20)
project(klblog)
enable_testing()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

if(CMAKE_BUILD_TYPE MATCHES Release)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif(CMAKE_BUILD_TYPE MATCHES Release)

if(COVERAGE_ENABLED)
  set(CMAKE_CXX_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
endif(COVERAGE_ENABLED)

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)
