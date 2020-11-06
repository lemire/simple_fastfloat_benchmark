#ifndef STRING_FORMAT_H
#define STRING_FORMAT_H

#include <iostream>
#include <sstream>
/**
 * This will generate a string with exactly the number of digits
 * that are required to always be able to recover the original
 * number (irrespective of the number). So 17 digits in the case
 * of a double.
 * E.g., 3.7018502067730191e-02
 */
template <typename T> std::string accurate_to_string(T d) {
  std::string answer;
  answer.resize(64);
  auto written = std::snprintf(answer.data(), 64, "%.*e",
                               std::numeric_limits<T>::max_digits10 - 1, d);
  if(written > 24) { abort(); }
  answer.resize(written);
  return answer;
}
template <typename T> std::string integer_to_string(T d) {
    std::stringstream ss;
    ss << d;
    return ss.str();
}

namespace internal {
  char *to_chars(char *first, const char *last, double value);
}

template <typename T> std::string accurate_to_string_concise(T d) {
  std::string answer;
  answer.resize(64);
  auto result = internal::to_chars(answer.data(), answer.data() + 64, d);
  if(result == answer.data()) { abort(); }
  if(result - answer.data() > 24) { abort(); }
  answer.resize(result - answer.data());
  return answer;
}

#endif