add_library(
  klbase
  klexcept.cpp
  klflags.hpp
  klfs.hpp
  klio.cpp
  klnet.cpp
  klprocess.cpp
  kltext.cpp
  kltimecounter.cpp
  kltime.cpp
  klvalue.cpp
  klexcept.hpp
  klfs.cpp
  kl.hpp
  klio.hpp
  klnet.hpp
  klprocess.hpp
  kltext.hpp
  kltimecounter.hpp
  kltime.hpp
  klvalue.hpp
  klds.hpp
  textscanner.cpp
  textscanner.hpp)
target_include_directories(klbase PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(klbase fmt::fmt)
