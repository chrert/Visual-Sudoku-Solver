cmake_minimum_required(VERSION 2.8)

project(vsudoku)

add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE "DEBUG")

find_package(OpenCV REQUIRED)

add_executable(vsudoku main.cpp sudokufinder.cpp)
target_link_libraries(vsudoku ${OpenCV_LIBS})

