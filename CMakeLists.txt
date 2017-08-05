cmake_minimum_required (VERSION 3.8.2)
project (reversi_exp)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow")

enable_testing()

add_subdirectory (reversi)
add_subdirectory (strategy)
add_subdirectory (genetics)
add_subdirectory (gothello)
add_subdirectory (reversi-cl)
add_subdirectory (reversigp)
add_subdirectory (test)

