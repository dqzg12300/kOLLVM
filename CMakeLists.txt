#cmake_minimum_required(VERSION 3.15)
#project(OLLVM)
#
#set(CMAKE_CXX_STANDARD 14)
#
#add_executable(OLLVM main.cpp)

cmake_minimum_required(VERSION 3.15.3)
project(OLLVM)
set(LLVM_DIR /home/king/ollvm/llvm-project-llvmorg-9.0.1/llvm/cmake-build-debug/lib/cmake/llvm)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(ollvm/include)
add_subdirectory(ollvm)
