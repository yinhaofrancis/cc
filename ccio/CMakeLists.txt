cmake_minimum_required(VERSION 3.20.0)
project(ccio VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(cc)

add_executable(ccio
main.cpp
)


target_link_libraries(ccio cc ccl)



