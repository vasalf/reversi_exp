cmake_minimum_required (VERSION 3.8.0)
project (gothello)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library (gothello STATIC
  gothello.cpp
  tournament.cpp
  elo.cpp
  sigsegv.cpp
  rsf.cpp)
target_link_libraries (gothello dl)
