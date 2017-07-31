cmake_minimum_required (VERSION 3.8.0)
project (reversi-cl)

find_package (gflags REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable (reversi-cl reversi-cl.cpp)
target_link_libraries (reversi-cl
  reversi
  strategy
  gflags)
