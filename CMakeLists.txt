cmake_minimum_required(VERSION 3.15)

project(SimpleFastFloatBenchmark VERSION 0.1.0 LANGUAGES CXX)
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  abseil
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_SHALLOW TRUE)

set(ABSL_RUN_TEST OFF CACHE INTERNAL "")
set(ABSL_USE_GOOGLETEST_HEAD OFF CACHE INTERNAL "")

#add_executable(repro main.cpp)
#target_link_libraries(repro PUBLIC simdjson)
