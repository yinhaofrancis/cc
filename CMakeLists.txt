cmake_minimum_required(VERSION 3.18.0)
project(run VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)


add_subdirectory(cc)
add_executable(kq
 main.cpp 
 )

 target_link_libraries(kq cc)