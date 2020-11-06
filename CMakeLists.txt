cmake_minimum_required(VERSION 3.15)

project(SimpleFastFloatBenchmark VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

include(FetchContent)
set(ABSL_ENABLE_INSTALL ON)
set(ABSL_RUN_TEST OFF CACHE INTERNAL "")
set(ABSL_USE_GOOGLETEST_HEAD OFF CACHE INTERNAL "")


FetchContent_Declare(abseil 
     GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git 
     GIT_TAG "20200225.2")
FetchContent_GetProperties(abseil)
if(NOT abseil_POPULATED)
    set(BUILD_TESTING OFF)
    FetchContent_Populate(abseil)
    add_subdirectory(${abseil_SOURCE_DIR} ${abseil_BINARY_DIR})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${abseil_SOURCE_DIR}/absl/copts)
    include(${abseil_SOURCE_DIR}/absl/copts/AbseilConfigureCopts.cmake)
endif()



FetchContent_Declare(
  fast_float
  GIT_REPOSITORY https://github.com/lemire/fast_float.git
  GIT_TAG origin/main
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(fast_float)

FetchContent_Declare(doubleconversion 
     GIT_REPOSITORY https://github.com/google/double-conversion.git 
     GIT_TAG "v3.1.5")
FetchContent_GetProperties(doubleconversion)
FetchContent_MakeAvailable(doubleconversion)



add_subdirectory(benchmarks)
