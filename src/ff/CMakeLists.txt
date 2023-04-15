add_library(klfileformats
    codescanner.cpp
    codescanner.hpp
    poorconfig.cpp
    poorconfig.hpp
    textscanner.cpp
    textscanner.hpp
    markwrite.cpp
    markwrite.hpp
    mwblocks.hpp)

target_include_directories(klfileformats PRIVATE ${CMAKE_SOURCE_DIR}/src)
