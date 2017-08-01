cmake_minimum_required (VERSION 3.8.0)
project (strategy)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library (strategy STATIC
  random.cpp
  scorings.cpp)
