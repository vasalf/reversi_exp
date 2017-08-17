cmake_minimum_required (VERSION 3.8.0)
project (reversigp)

find_package (gflags REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable (reversigp reversigp.cpp)
target_link_libraries (reversigp
  gothello
  reversi
  genetics
  strategy
  gflags)
