cmake_minimum_required(VERSION 3.10)

# Set the project name
project(rock_splits)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add the executable
add_executable(rock_splits src/main.cpp)