
#include "fast_float/parse_number.h"

#include <cassert>
#include <cmath>

template <typename T> char *to_string(T d, char *buffer) {
  auto written = std::snprintf(buffer, 128, "%.*e",
                               64, d);
  return buffer + written;
}

void allvalues() {
  char buffer[128];
  for (uint64_t w = 0; w <= 0xFFFFFFFF; w++) {
    float v;
    if ((w % 1048576) == 0) {
      std::cout << ".";
      std::cout.flush();
    }
    uint32_t word = uint32_t(w);
    memcpy(&v, &word, sizeof(v));

    //if (std::isnormal(v))
    {
      const char *string_end = to_string(v, buffer);
      float result_value;
      auto result = fastfloat::from_chars(buffer, string_end, result_value);
      if (result.ec != std::errc()) {
        std::cerr << "parsing error ? " << buffer << std::endl;
        abort();
      }
      if (std::isnan(v)) {
        if (!std::isnan(result_value)) {
          std::cerr << "not nan" << buffer << std::endl;
          abort();
        }
      } else if (result_value != v) {
        std::cerr << "no match ? " << buffer << " got " <<  result_value << " expected " << v << std::endl;
        abort();
      }
    }
  }
  std::cout << std::endl;
}

int main() {
  allvalues();
  std::cout << std::endl;
  std::cout << "all ok" << std::endl;
  return EXIT_SUCCESS;
}