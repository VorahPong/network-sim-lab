cmake_minimum_required(VERSION 3.16)

project(NetworkSimLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sim_node src/sim_node.cpp)