# simple_fastfloat_benchmark

Your compiler needs to support C++17.

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



## Example output

Visual Studio 2019:

```
$ ./build/benchmarks/Release/benchmark.exe
# parsing random integers in the range [0,1)
strtod                                  :   118.01 MB/s (+/- 2.4 %)
abseil                                  :   294.71 MB/s (+/- 1.6 %)
fastfloat                               :   608.39 MB/s (+/- 1.3 %)
from_chars                              :   137.21 MB/s (+/- 0.9 %)
# You can also provide a filename: it should contain one string per line corresponding to a number
```

GNU  GCC 9 (Linux):
```
$ ./build/benchmarks/benchmark
# parsing random integers in the range [0,1)
strtod                                  :   183.82 MB/s (+/- 2.5 %)
abseil                                  :   450.62 MB/s (+/- 2.5 %)
fastfloat                               :  1086.27 MB/s (+/- 2.5 %)
# You can also provide a filename: it should contain one string per line corresponding to a number
```
