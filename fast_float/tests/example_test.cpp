
#include "fast_float/parse_number.h"
#include <iostream>
 
int main() {
    const std::string input =  "3.1416 xyz ";
    double result;
    auto answer = fastfloat::from_chars(input.data(), input.data()+input.size(), result);
    if(answer.ec != std::errc()) { std::cerr << "parsing failure\n"; return EXIT_FAILURE; }
    std::cout << "parsed the number " << result << std::endl;
}
