cmake_minimum_required(VERSION 3.24)
project(Spearfish)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(spearfish src/cpp/main.cpp)
find_package(OpenMP)
target_link_libraries(spearfish PRIVATE OpenMP::OpenMP_CXX)
