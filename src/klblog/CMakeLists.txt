add_executable(klblog blog.cpp blog.hpp blogsettings.cpp blogsettings.hpp systemsettings.cpp systemsettings.hpp klblog.cpp)
target_include_directories(klblog PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(klblog klfileformats klbase)