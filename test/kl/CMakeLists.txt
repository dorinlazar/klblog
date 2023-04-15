add_executable(kltests kldate-tests.cpp klfs-tests.cpp kltext-tests.cpp klvalue-tests.cpp)
target_link_libraries(kltests GTest::gtest_main klbase)
target_include_directories(kltests PRIVATE ${CMAKE_SOURCE_DIR}/src)
