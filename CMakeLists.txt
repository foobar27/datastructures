project(datastructures)
cmake_minimum_required(VERSION 2.8)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

add_subdirectory(test)
add_subdirectory(benchmarks)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
SET(CMAKE_CXX_FLAGS "-std=c++0x")


