cmake_minimum_required(VERSION 3.7)
project(ModeleXXL)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 11 ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})

set(SOURCE_FILES main.cpp Lamport.cpp Lamport.h Agent.cpp Agent.h)
add_executable(ModeleXXL ${SOURCE_FILES})