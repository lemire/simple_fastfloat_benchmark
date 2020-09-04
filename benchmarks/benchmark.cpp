
#include "absl/strings/charconv.h"
#include "absl/strings/numbers.h"
#include "parse_number.h"

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>


double findmax_strtod(std::vector<std::string> &s) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    char *pr = (char *)st.data();
    x = strtod(st.data(), &pr);
    if ((pr == nullptr) || (pr == st.data())) {
      throw std::runtime_error("bug in findmax_strtod");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}


double findmax_fastfloat_from_chars(std::vector<std::string>& s) {
  double answer = 0;
  double x = 0;
  for (std::string& st : s) {
    auto[p, ec] = fastfloat::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data()) {
      throw std::runtime_error("bug in findmax_fastfloat_from_chars");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}

double findmax_absl_from_chars(std::vector<std::string> &s) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    auto[p, ec] = absl::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data()) {
      throw std::runtime_error("bug in findmax_absl_from_chars");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}

template<class T>
void time_it(std::vector<std::string>& lines, T function) {
    std::chrono::high_resolution_clock::time_point t1, t2;
    t1 = std::chrono::high_resolution_clock::now();
    double ts = T(lines);
    if (ts == 0) { printf("bug\n"); }
    t2 = std::chrono::high_resolution_clock::now();
    double dif = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    double dif =
}

void process(std::vector<std::string>& lines, size_t volume) {
  double volumeMB = volume / (1024. * 1024.);
  std::chrono::high_resolution_clock::time_point t1, t2;
  double dif, ts;
  for(size_t i = 0; i < 3; i++) {  
    ts = findmax_strtod(lines);
    t2 = std::chrono::high_resolution_clock::now();
    dif = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("strtod         %.2f MB/s\n", volumeMB * 1000000000 / dif);

    t1 = std::chrono::high_resolution_clock::now();
    ts = findmax_absl_from_chars(lines);
    if (ts == 0) { printf("bug\n"); }
    t2 = std::chrono::high_resolution_clock::now();
    dif = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("abslfromch     %.2f MB/s\n", volumeMB * 1000000000 / dif);

    t1 = std::chrono::high_resolution_clock::now();
    ts = findmax_fastfloat_from_chars(lines);
    if (ts == 0) { printf("bug\n"); }
    t2 = std::chrono::high_resolution_clock::now();
    dif = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    printf("fastfloatfromch     %.2f MB/s\n", volumeMB * 1000000000 / dif);


    printf("\n\n");
  }
}

void fileload(char *filename) {
  std::ifstream inputfile(filename);
  if (!inputfile) {
    std::cerr << "can't open " << filename << std::endl;
    return;
  }
  std::string line;
  std::vector<std::string> lines;
  lines.reserve(10000); // let us reserve plenty of memory.
  size_t volume = 0;
  while (getline(inputfile, line)) {
    volume += line.size();
    lines.push_back(line);
  }
  std::cout << "read " << lines.size() << " lines " << std::endl;
  process(lines, volume);
}

template <typename T> std::string accurate_to_string(T d) {
  std::string answer;
  answer.resize(64);
  auto written = std::snprintf(answer.data(), 64, "%.*e", std::numeric_limits<T>::max_digits10 - 1, d);
  answer.resize(written);
  return answer;
}

void demo(size_t howmany) {
  std::cout << "parsing random integers in the range [0,1)" << std::endl;
  std::vector<std::string> lines;
  lines.reserve(howmany); // let us reserve plenty of memory.
  size_t volume = 0;
  for (size_t i = 0; i < howmany; i++) {
    double x = (double)rand() / RAND_MAX;
    std::string line = accurate_to_string(x);
    volume += line.size();
    lines.push_back(line);
  }
  process(lines, volume);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    demo(100 * 1000);
    std::cout << "You can also provide a filename: it should contain one "
                 "string per line corresponding to a number"
              << std::endl;
  } else {
    fileload(argv[1]);
  }
}