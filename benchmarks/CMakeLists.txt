add_executable(benchmark benchmark.cpp to_chars.cpp)
add_executable(benchmark32 benchmark32.cpp to_chars.cpp)
set_property(
    TARGET benchmark benchmark32
    PROPERTY CXX_STANDARD 17)

if (NOT CYGWIN)
    target_link_libraries(benchmark32 PUBLIC absl::strings )
endif()
target_link_libraries(benchmark32 PUBLIC fast_float )
if(ENABLE_RYU)
    add_compile_definitions(ENABLE_RYU)
    target_link_libraries(benchmark32 PUBLIC ryu_parse)
endif()

if (NOT CYGWIN)
    target_link_libraries(benchmark PUBLIC absl::strings )
endif()
target_link_libraries(benchmark PUBLIC fast_float)
target_link_libraries(benchmark PUBLIC double-conversion)
if(ENABLE_RYU)
    target_link_libraries(benchmark PUBLIC ryu_parse)
endif()
