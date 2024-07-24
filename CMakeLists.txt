cmake_minimum_required(VERSION 3.24)
project(Spearfish)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#find_package(OpenMP)
#target_link_libraries(spearfish PRIVATE OpenMP::OpenMP_CXX)

# Code used for tests and final executable
add_library(spearfish_code STATIC
        src/cpp/io/args.cpp
        src/cpp/io/newick.cpp
        src/cpp/io/exec_cmd.cpp
        src/cpp/misc/matrix.hpp)

# Spearfish
add_executable(spearfish src/cpp/main.cpp)
target_link_libraries(spearfish spearfish_code)

# Testing
find_package(ut REQUIRED)

add_executable(spearfish_tests test/cpp/main.cpp test/cpp/misc/matrix_test.cpp)
target_link_libraries(spearfish_tests spearfish_code Boost::ut)
