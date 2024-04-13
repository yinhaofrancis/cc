cmake_minimum_required(VERSION 3.28.0)
project(rpc_test VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(rpc)

add_executable(rpc_test main.cpp)


target_link_libraries(rpc_test rpc)
