cmake_minimum_required(VERSION 3.20.0)
project(rpc_test VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(ipc)

add_executable(ipc_test main.cpp)


target_link_libraries(ipc_test ipc)
