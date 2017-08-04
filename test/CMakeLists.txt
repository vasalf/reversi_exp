cmake_minimum_required (VERSION 3.8.0)
project (test)

include (FindGTest)
find_package (GTest REQUIRED)
include_directories (${GTEST_INCLUDE_DIRS})
link_libraries (GTest::GTest)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable (test_genotype test_genotype.cpp)
target_link_libraries (test_genotype genetics)

add_test (Genotype test_genotype)
