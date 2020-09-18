#include "fast_float/fast_float.h"
#include <iomanip>

inline void Assert(bool Assertion) {
  if (!Assertion)
    throw std::runtime_error("bug");
}

template <typename T> std::string to_string(T d) {
  std::string s(64, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%.*e",
                               std::numeric_limits<T>::max_digits10 - 1, d);
  s.resize(written);
  return s;
}

template <typename T> std::string to_long_string(T d) {
  std::string s(4096, '\0');
  auto written = std::snprintf(&s[0], s.size(), "%.*e",
                               std::numeric_limits<T>::max_digits10 * 10, d);
  s.resize(written);
  return s;
}

bool basic_test_32bit(std::string vals) {
  std::cout << " parsing "  << vals << std::endl;
  float result_value;
  auto result = fast_float::from_chars(vals.data(), vals.data() + vals.size(),
                                      result_value);
  if (result.ec != std::errc()) {
    std::cerr << " I could not parse " << vals << std::endl;
    return false;
  }

  std::cout << std::hexfloat << result_value << std::endl;
  std::cout << std::dec;
  return true;
}

bool basic_test_32bit(std::string vals, float val) {
  std::cout << " parsing "  << vals << std::endl;
  float result_value;
  auto result = fast_float::from_chars(vals.data(), vals.data() + vals.size(),
                                      result_value);
  if (result.ec != std::errc()) {
    std::cerr << " I could not parse " << vals << std::endl;
    return false;
  }
  if (std::isnan(val)) {
    if (!std::isnan(result_value)) {
      std::cerr << "not nan" << result_value << std::endl;
      return false;
    }
  } else if (result_value != val) {
    std::cerr << "I got " << std::setprecision(15) << result_value << " but I was expecting " << val
              << std::endl;
    uint32_t word;
    memcpy(&word, &result_value, sizeof(word));
    std::cout << "got mantissa = " << (word & ((1<<23)-1)) << std::endl;
    memcpy(&word, &val, sizeof(word));
    std::cout << "wanted mantissa = " << (word & ((1<<23)-1)) << std::endl;
    std::cerr << "string: " << vals << std::endl;
    return false;
  }
  std::cout << std::hexfloat  << result_value << " == " << val << std::endl;
  std::cout << std::dec;
  return true;
}

bool basic_test_32bit(float val) {
  std::string long_vals = to_long_string(val);
  std::string vals = to_string(val);
  return basic_test_32bit(long_vals, val) && basic_test_32bit(vals, val);
}

bool basic_test_64bit(std::string vals, double val) {
  std::cout << " parsing "  << vals << std::endl;
  double result_value;
  auto result = fast_float::from_chars(vals.data(), vals.data() + vals.size(),
                                      result_value);
  if (result.ec != std::errc()) {
    std::cerr << " I could not parse " << vals << std::endl;
    return false;
  }
  if (std::isnan(val)) {
    if (!std::isnan(result_value)) {
      std::cerr << "not nan" << result_value << std::endl;
      return false;
    }
  } else if (result_value != val) {
    std::cerr << "I got " << std::setprecision(15) << result_value << " but I was expecting " << val
              << std::endl;
    std::cerr << "string: " << vals << std::endl;
    return false;
  }
  std::cout << std::hexfloat << result_value << " == " << val << std::endl;
  std::cout << std::dec;

  return true;
}
bool basic_test_64bit(double val) {
  std::string long_vals = to_long_string(val);
  std::string vals = to_string(val);
  return basic_test_64bit(long_vals, val) && basic_test_64bit(vals, val);
}

int main() {
  std::cout << "======= 32 bits " << std::endl;
  Assert(basic_test_32bit("+1", 1));
  Assert(basic_test_32bit("2e3000", std::numeric_limits<float>::infinity()));
  Assert(basic_test_32bit("3.5028234666e38", std::numeric_limits<float>::infinity()));
  Assert(basic_test_32bit("7.0060e-46", 0));
  Assert(basic_test_32bit(1.00000006e+09f));
  Assert(basic_test_32bit(1.4012984643e-45f));
  Assert(basic_test_32bit(1.1754942107e-38f));
  Assert(basic_test_32bit(1.1754943508e-45f));
  Assert(basic_test_32bit(3.4028234664e38f));
  Assert(basic_test_32bit(3.4028234665e38f));
  Assert(basic_test_32bit(3.4028234666e38f));
  std::cout << std::endl;

  std::cout << "======= 64 bits " << std::endl;
  Assert(basic_test_64bit("+1", 1));
  Assert(basic_test_64bit("2e3000", std::numeric_limits<double>::infinity()));
  Assert(basic_test_64bit("1.9e308", std::numeric_limits<double>::infinity()));
  Assert(basic_test_64bit(3e-324));
  Assert(basic_test_64bit(1.00000006e+09f));
  Assert(basic_test_64bit(4.9406564584124653e-324));
  Assert(basic_test_64bit(4.9406564584124654e-324));
  Assert(basic_test_64bit(2.2250738585072009e-308));
  Assert(basic_test_64bit(2.2250738585072014e-308));
  Assert(basic_test_64bit(1.7976931348623157e308));
  Assert(basic_test_64bit(1.7976931348623158e308));
  std::cout << std::endl;
  std::cout << "All ok" << std::endl;
  return EXIT_SUCCESS;
}
