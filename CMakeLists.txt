cmake_minimum_required(VERSION 3.15)

project(SimpleFastFloatBenchmark VERSION 0.1.0 LANGUAGES CXX)
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(ABSL_ENABLE_INSTALL ON)
set(ABSL_RUN_TEST OFF CACHE INTERNAL "")
set(ABSL_USE_GOOGLETEST_HEAD OFF CACHE INTERNAL "")

#FetchContent_Declare(
#    absl
#    GIT_REPOSITORY "https://github.com/abseil/abseil-cpp.git"
#    GIT_TAG "20200225.2"
#    PATCH_COMMAND git apply "${CMAKE_CURRENT_LIST_DIR}/patches/abseil-cpp-20200225.2.patch")
#FetchContent_MakeAvailable(absl)
FetchContent_Declare(abseil GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git GIT_TAG origin/master)
FetchContent_GetProperties(abseil)
if(NOT abseil_POPULATED)
    set(BUILD_TESTING OFF)
    FetchContent_Populate(abseil)
    add_subdirectory(${abseil_SOURCE_DIR} ${abseil_BINARY_DIR})
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${abseil_SOURCE_DIR}/absl/copts)
    include(${abseil_SOURCE_DIR}/absl/copts/AbseilConfigureCopts.cmake)
endif()



add_subdirectory(fast_float)
add_subdirectory(benchmarks)
