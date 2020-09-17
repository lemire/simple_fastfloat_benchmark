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
volume = 2.09808 MB
netlib                                  :   346.95 MB/s (+/- 7.1 %)
strtod                                  :   119.96 MB/s (+/- 3.6 %)
abseil                                  :   304.02 MB/s (+/- 7.7 %)
fastfloat                               :   630.15 MB/s (+/- 6.9 %)
from_chars                              :   142.41 MB/s (+/- 3.1 %)
# You can also provide a filename: it should contain one string per line corresponding to a number

$ ./build/benchmarks/Release/benchmark.exe data/canada.txt
# read 111126 lines
volume = 1.93374 MB
netlib                                  :   354.43 MB/s (+/- 5.6 %)
strtod                                  :   105.37 MB/s (+/- 2.3 %)
abseil                                  :   268.35 MB/s (+/- 3.2 %)
fastfloat                               :   533.79 MB/s (+/- 3.8 %)
from_chars                              :   119.55 MB/s (+/- 2.6 %)

$ ./build/benchmarks/Release/benchmark.exe data/mesh.txt
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
strtod                                  :   183.82 MB/s (+/- 2.5 %)
abseil                                  :   450.62 MB/s (+/- 2.5 %)
fastfloat                               :  1086.27 MB/s (+/- 2.5 %)
# You can also provide a filename: it should contain one string per line corresponding to a number
```
