add_executable(benchmark benchmark.cpp to_chars.cpp)


target_link_libraries(benchmark PUBLIC absl::strings )
target_link_libraries(benchmark PUBLIC fast_float)
target_link_libraries(benchmark PUBLIC double-conversion)