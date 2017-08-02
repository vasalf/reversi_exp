cmake_minimum_required (VERSION 3.8.0)
project (genetics)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library (genetics STATIC
  genotype.cpp)
