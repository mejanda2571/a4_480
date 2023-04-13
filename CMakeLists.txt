cmake_minimum_required(VERSION 3.25)
project(A4)

set(CMAKE_CXX_STANDARD 11)

add_executable(A4 main.cpp Producer.cpp Producer.h Consumer.cpp Consumer.h SharedData.cpp SharedData.h)
