# simple_fastfloat_benchmark

This repository contains benchmarking code related to the [fast_float](https://github.com/lemire/fast_float.git) number parsing library.
It supports at least Linux, macOS and Windows (Visual Studio).


If you have a recent version of CMake (3.15 or better) under linux, macOS or freeBSD,  you can simply
go in the directory and type the following commands:

```
cmake -B build .
cmake --build build
./build/benchmarks/benchmark 
```

Under Windows, the process is similar:

```
cmake -B build .
cmake --build build --config Release
./build/benchmarks/Release/benchmark.exe
```


While the `fast_float` library only requires C++11, for this benchmark, we are assuming that your compiler supports C++17.

## Advanced Usage

Parse the strings (one per line) included in a text file:

```
./build/benchmarks/benchmark -f data/canada.txt
```

Parse strings generated from floats in (0,1):


```
./build/benchmarks/benchmark
```

Parse strings generated from floats in (0,1), using a few digits as possible during the serialization:

```
./build/benchmarks/benchmark -c
```


Parse strings generated from floats in the `int_e_int`model (credit @alexey-milovidov), using a few digits as possible during the serialization:

```
./build/benchmarks/benchmark -m int_e_int
```

There a different models available, they are printed out in the console.

## Example output

Visual Studio 2019:

```
$ ./build/benchmarks/Release/benchmark.exe
# parsing random integers in the range [0,1)
volume = 2.09808 MB
netlib                                  :   346.95 MB/s (+/- 7.1 %)
strtod                                  :   119.96 MB/s (+/- 3.6 %)
abseil                                  :   304.02 MB/s (+/- 7.7 %)
fastfloat                               :   630.15 MB/s (+/- 6.9 %)
from_chars                              :   142.41 MB/s (+/- 3.1 %)
# You can also provide a filename: it should contain one string per line corresponding to a number

$ ./build/benchmarks/Release/benchmark.exe -f data/canada.txt
# read 111126 lines
volume = 1.93374 MB
netlib                                  :   354.43 MB/s (+/- 5.6 %)
strtod                                  :   105.37 MB/s (+/- 2.3 %)
abseil                                  :   268.35 MB/s (+/- 3.2 %)
fastfloat                               :   533.79 MB/s (+/- 3.8 %)
from_chars                              :   119.55 MB/s (+/- 2.6 %)

$ ./build/benchmarks/Release/benchmark.exe -f  data/mesh.txt
# read 73019 lines
volume = 0.536009 MB
netlib                                  :   329.79 MB/s (+/- 5.4 %)
strtod                                  :    84.78 MB/s (+/- 1.6 %)
abseil                                  :   145.67 MB/s (+/- 1.4 %)
fastfloat                               :   476.37 MB/s (+/- 1.9 %)
from_chars                              :   104.53 MB/s (+/- 2.0 %)
```

GNU  GCC 9 (Linux):
```
$ ./build/benchmarks/benchmark 
# parsing random integers in the range [0,1)
volume = 2.09808 MB 
netlib                                  :   271.18 MB/s (+/- 1.2 %)    12.93 Mfloat/s  
doubleconversion                        :   225.35 MB/s (+/- 1.2 %)    10.74 Mfloat/s  
strtod                                  :   190.94 MB/s (+/- 1.6 %)     9.10 Mfloat/s  
abseil                                  :   430.45 MB/s (+/- 2.2 %)    20.52 Mfloat/s  
fastfloat                               :  1042.38 MB/s (+/- 9.9 %)    49.68 Mfloat/s  

$ ./build/benchmarks/benchmark -f data/canada.txt 
# read 111126 lines 
volume = 1.93374 MB 
netlib                                  :   287.92 MB/s (+/- 1.5 %)    16.55 Mfloat/s  
doubleconversion                        :   203.95 MB/s (+/- 1.4 %)    11.72 Mfloat/s  
strtod                                  :   139.51 MB/s (+/- 2.0 %)     8.02 Mfloat/s  
abseil                                  :   399.62 MB/s (+/- 1.7 %)    22.96 Mfloat/s  
fastfloat                               :   912.10 MB/s (+/- 2.3 %)    52.42 Mfloat/s  

$ ./build/benchmarks/benchmark -f data/mesh.txt 
# read 73019 lines 
volume = 0.536009 MB 
netlib                                  :   309.97 MB/s (+/- 2.8 %)    42.23 Mfloat/s  
doubleconversion                        :   221.93 MB/s (+/- 1.5 %)    30.23 Mfloat/s  
strtod                                  :   117.69 MB/s (+/- 1.8 %)    16.03 Mfloat/s  
abseil                                  :   243.55 MB/s (+/- 2.8 %)    33.18 Mfloat/s  
fastfloat                               :   657.04 MB/s (+/- 4.6 %)    89.51 Mfloat/s  
```
