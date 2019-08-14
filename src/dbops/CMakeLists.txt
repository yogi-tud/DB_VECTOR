cmake_minimum_required(VERSION 3.12)  # CMake version check
project(DB_VEC)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

add_compile_options(-mavx512cd)
add_compile_options(-mavx512f)
add_compile_options(-mavx512vl)

# Add main.cpp file of project root directory as source file

include_directories(/opt/nec/ve/veos/include)
include_directories(/home/fett/code)


add_executable(mpsync sync_vhmp.cpp)

