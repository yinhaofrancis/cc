if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(../cmake/cross_compile.cmake)
endif()

cmake_minimum_required(VERSION 3.20.0)
project(cc VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
project(ccm VERSION 0.1.0 LANGUAGES C CXX)

add_library(cc
ref.cpp ref.hpp 
block.cpp block.hpp 
stream.hpp stream.cpp 
socket.cpp socket.hpp 
task.hpp task.cpp
poll.cpp poll.hpp
server.cpp server.hpp
connection.cpp connection.hpp
)

if(APPLE)
    add_library(ccm
        event.hpp event.cpp
    )
else()
    add_library(ccm
        epoll.hpp epoll.cpp
    )
endif()
