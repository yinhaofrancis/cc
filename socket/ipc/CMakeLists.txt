cmake_minimum_required(VERSION 3.20.0)
project(rpc VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_library(
    ipc 
    socket.hpp socket.cpp 
    stream.hpp stream.cpp 
    define.hpp define.cpp
    select.hpp
)
