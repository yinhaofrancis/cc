cmake_minimum_required(VERSION 3.18.0)
project(cc VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(kq
 main.cpp 
 task.cpp
 task.hpp
 event.cpp
 event.hpp
 )
