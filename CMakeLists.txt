cmake_minimum_required(VERSION 3.10)

# set project
project(aOpenGL VERSION 1.0.0)
set(CMAKE_CXX_COMPLIER "g++")
set(CMAKE_CXX_STANDARD 17)

# set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# options
#add_compile_options(-Wall)
#add_compile_options(-Wno-unused-variable)
add_compile_options(-fPIC)

# compile option
add_compile_definitions(AGL_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

# set directories
set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)

# source codes
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/code)

# example codes
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)