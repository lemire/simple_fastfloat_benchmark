add_executable(benchmark benchmark.cpp to_chars.cpp)
add_executable(benchmark32 benchmark32.cpp to_chars.cpp)

target_link_libraries(benchmark32 PUBLIC absl::strings )
target_link_libraries(benchmark32 PUBLIC fast_float )
target_link_libraries(benchmark32 PUBLIC ryu_parse)

target_link_libraries(benchmark PUBLIC absl::strings )
target_link_libraries(benchmark PUBLIC fast_float)
target_link_libraries(benchmark PUBLIC double-conversion)
target_link_libraries(benchmark PUBLIC ryu_parse)
