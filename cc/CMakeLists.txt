cmake_minimum_required(VERSION 3.18.0)
project(cc VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
add_library(
    cc
    stream.cpp
    stream.hpp
    pipe.hpp
    pipe.cpp
    task.cpp
    task.hpp
    event.cpp
    event.hpp
    select.hpp
    select.cpp
    time.hpp
    tcp.cpp
    tcp.hpp
)
else()
add_library(
    cc
    stream.cpp
    stream.hpp
    pipe.hpp
    pipe.cpp
    task.cpp
    task.hpp
    select.hpp
    select.cpp
    time.hpp
    tcp.cpp
    tcp.hpp
)
endif()

