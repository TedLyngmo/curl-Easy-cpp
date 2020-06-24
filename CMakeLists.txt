cmake_minimum_required (VERSION 3.15)
project(curleasy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src lib)
add_subdirectory(demo)
