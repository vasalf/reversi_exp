cmake_minimum_required (VERSION 3.8.2)
project (reversi)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library (reversi
            field.cpp)
            
