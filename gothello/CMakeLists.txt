cmake_minimum_required (VERSION 3.8.0)
project (gothello)

find_package (RapidJSON REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library (gothello STATIC
  gothello.cpp
  tournament.cpp
  elo.cpp
  sigsegv.cpp
  rsf.cpp)
target_include_directories (gothello PUBLIC
  ${RapidJSON_INCLUDE_DIRS})
target_link_libraries (gothello dl)
