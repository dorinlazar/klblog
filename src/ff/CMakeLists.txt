add_library(klfileformats poorconfig.cpp poorconfig.hpp markwrite.cpp
                          markwrite.hpp mwblocks.hpp)

target_include_directories(klfileformats PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(klfileformats kl)
