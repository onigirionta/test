cmake_minimum_required(VERSION 3.12)
project(tes)

set(CMAKE_CXX_STANDARD 17)

add_executable(tes main.cpp)
add_executable(test_parser test_parser.cpp)
add_executable(readCOM read.cpp)

enable_testing()
add_test(test_parser COMMAND test_parser)