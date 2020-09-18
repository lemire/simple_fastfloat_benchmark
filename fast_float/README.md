## fast_float number parsing library

The fast_float library provides fast implementation for the following two
functions with a C++17-like syntax:

```C++
from_chars_result from_chars(const char* first, const char* last, float& value, ...);
from_chars_result from_chars(const char* first, const char* last, double& value, ...);
```

The return type (`from_chars_result`) is defined as the struct:
```C++
struct from_chars_result {
    const char* ptr;
    std::errc ec;
};
```

It parses the character sequence [first,last) for number. It parses floating-point numbers expecting
a locale-indepent format equivalent to what is used by std::strtod in the default ("C") locale. 
The resulting floating-point value is the closest floating-point values (using either float or double), 
using the "round to even" convention for values that would otherwise fall right in-between two values.
That is, we provide exact parsing according to the IEEE standard.

Given a successful parse, the pointer (`ptr`) in the returned value is set to point right after the
parsed number, and the `value` referenced is set to the parsed value. In case of error, the returned
`ec` contains a representative error, otherwise the default (`std::errc()`) value is stored.

The implementation do not throw and do not allocate memory (e.g., with `new` or `malloc`).

Example:

``` C++
#include "fast_float/parse_number.h"
#include <iostream>
 
int main() {
    const std::string input =  "3.1416 xyz ";
    double result;
    auto answer = fast_float::from_chars(input.data(), input.data()+input.size(), result);
    if(answer.ec != std::errc()) { std::cerr << "parsing failure\n"; return EXIT_FAILURE; }
    std::cout << "parsed the number " << result << std::endl;
}
```

Like the C++17 standard, the `fast_float::from_chars` functions take an optional last argument of
the type `fast_float::chars_format`. It is a bitset value: we check whether 
`fmt & fast_float::chars_format::fixed` and `fmt & fast_float::chars_format::scientific` are set
to determined whether we allowed the fixed point and scientific notation respectively.
The default is  `fast_float::chars_format::general` which allows both `fixed` and `scientific`.

## Requirements and Limitations

In many cases, this library can be used as a drop-in replacement for the C++17 `from_chars` function, especially
when performance is a concerned. Thus we expect C++17 support. Though it might be reasonable to want
C++17 features as part of older compilers, it is not an objective of this library.

The `from_chars` is meant to be locale-independent. Thus it is not an objective of this library to support
locale-sensitive parsing.