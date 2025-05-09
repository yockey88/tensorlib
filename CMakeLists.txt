cmake_minimum_required(VERSION 3.10.0..4.0.1 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_CUDA_STANDARD 23)
# set(CUDA_USE_STATIC_CUDA_RUNTIME ON)
# set(CMAKE_CUDA_ARCHITECTURES "89")

project(tensorlib LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TENSORLIB_EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
set(TENSORLIB_EXTERN_INCLUDES ${TENSORLIB_EXTERN_DIR}/gcem)

set(TENSORLIB_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/tensorlib ${TENSORLIB_EXTERN_DIR})

option(TENSORLIB_REAL_TYPE_USE_DOUBLE "Use doubles for tensor values" OFF)


### add dependencies
find_package(Threads REQUIRED)

add_subdirectory(extern)

add_subdirectory(tensorlib)
# add_subdirectory(gpu)
add_subdirectory(neural)

add_subdirectory(test)
