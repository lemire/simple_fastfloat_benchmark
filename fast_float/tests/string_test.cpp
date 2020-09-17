#include "fast_float/parse_number.h"
#include <vector>

inline void Assert(bool Assertion) {
  if (!Assertion)
    throw std::runtime_error("bug");
}

template <typename T>
bool test() {
  std::string input = "0.1 1e1000 100000 3.14159265359  -1e-500 001    1e01  1e0000001  -inf";
  std::vector<T> answers = {T(0.1), std::numeric_limits<T>::infinity(), 100000, T(3.14159265359),  -0.0, 1,    10,  10, -std::numeric_limits<T>::infinity()};
  const char * begin = input.data();
  const char * end = input.data() + input.size();
  for(size_t i = 0; i < answers.size(); i++) {
    T result_value;
    auto result = fastfloat::from_chars(begin, end,
                                      result_value);
    if (result.ec != std::errc()) {
      printf("parsing %.*s\n", int(end - begin), begin);
      std::cerr << " I could not parse " << std::endl;
      return false;
    }
    if(result_value != answers[i]) {
      printf("parsing %.*s\n", int(end - begin), begin);
      std::cerr << " Mismatch " << std::endl;
      return false;

    }
    begin = result.ptr;
  }
  if(begin != end) {
      std::cerr << " bad ending " << std::endl;
      return false;    
  }
  return true;
}

int main() {

  std::cout << "32 bits checks" << std::endl;
  Assert(test<float>());

  std::cout << "64 bits checks" << std::endl;
  Assert(test<double>());

  std::cout << "All ok" << std::endl;
  return EXIT_SUCCESS;
}
