cmake_minimum_required(VERSION 3.15)

project(fast_float VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()


add_library(fast_float INTERFACE)
target_include_directories(fast_float INTERFACE include/)


enable_testing()

add_subdirectory(tests)
