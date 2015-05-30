cmake_minimum_required(VERSION 3.1)
project(threading-examples)

set(CMAKE_CXX_FLAGS "-g -Wall -Wpedantic -pthread")
find_package(Threads REQUIRED)

add_executable(philosophers philosophers.cpp)
add_executable(readers_writers readers_writers.cpp)
set_property(TARGET
	philosophers
	readers_writers
PROPERTY CXX_STANDARD 11)
