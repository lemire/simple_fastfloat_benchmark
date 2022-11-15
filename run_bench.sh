#!/bin/bash

declare -a StringArray=("-f data/canada.txt" "-f data/canada_short.txt" "-f data/mesh.txt" "-m uniform" "-m uniform -c" "-m simple_uniform32" "-m simple_uniform32 -c" "-m simple_int32" )

#cmake -B build --parallel
#cmake --build build --parallel
for ((i = 0; i < ${#StringArray[@]}; i++))
do
    echo "${StringArray[$i]}"
   ./build/benchmarks/benchmark ${StringArray[$i]}
done
