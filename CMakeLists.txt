cmake_minimum_required(VERSION 3.10)

project(MemoryManager)

set(CMAKE_CXX_STANDARD 14)

# Specify the source files to compile
add_executable(memory_manager src/main.cpp src/memory_manager.cpp)
