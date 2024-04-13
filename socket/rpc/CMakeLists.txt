cmake_minimum_required(VERSION 3.28.0)
project(rpc VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(
    rpc 
    socket.hpp stream.hpp stream.cpp socket.cpp define.hpp define.cpp
)
