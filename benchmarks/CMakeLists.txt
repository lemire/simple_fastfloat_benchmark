add_executable(benchmark benchmark.cpp)


target_link_libraries(benchmark PUBLIC absl::strings )
#-labsl_strings   -labsl_raw_logging_internal -labsl_throw_delegate -labsl_int128
target_link_libraries(benchmark PUBLIC fast_float)