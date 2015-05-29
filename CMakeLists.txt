cmake_minimum_required(VERSION 3.1)
project(threading-examples)

set(CMAKE_CXX_FLAGS "-g -Wall -Werror -pthread")
find_package(Threads REQUIRED)

add_executable(philosophers philosophers.cpp)
set_property(TARGET philosophers PROPERTY CXX_STANDARD 11)
