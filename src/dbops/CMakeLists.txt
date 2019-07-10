cmake_minimum_required(VERSION 3.12)  # CMake version check
project(simple_example)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard
SET(CMAKE_CXX_FLAGS -pthread)

# Add main.cpp file of project root directory as source file

add_compile_options(-pthread)
add_compile_options(-mavx512cd)
add_compile_options(-mavx512f)
add_compile_options(-mavx512vl)

set(SOURCE_FILES comp.c thread.h)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(simple_example ${SOURCE_FILES})