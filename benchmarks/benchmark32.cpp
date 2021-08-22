
#include "absl/strings/charconv.h"
#include "absl/strings/numbers.h"
#include "fast_float/fast_float.h"

#ifdef ENABLE_RYU
#include "ryu_parse.h"
#endif


#include "cxxopts.hpp"
#ifdef __linux__
#include "event_counter.h"
#endif
#include <algorithm>
#include <charconv>
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
#include <random>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <locale.h>

#include "random_generators.h"

/**
 * Determining whether we should import xlocale.h or not is
 * a bit of a nightmare.
 */
#ifdef __GLIBC__
#include <features.h>
#if !((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ > 25)))
#include <xlocale.h> // old glibc
#endif
#else            // not glibc
#ifndef _MSC_VER // assume that everything that is not GLIBC and not Visual
                 // Studio needs xlocale.h
#include <xlocale.h>
#endif
#endif
double findmax_strtof(std::vector<std::string> &s) {
  float answer = 0;
  float x = 0;
  for (std::string &st : s) {
    char *pr = (char *)st.data();
#ifdef _WIN32
    static _locale_t c_locale = _create_locale(LC_ALL, "C");
    x = _strtof_l(st.data(), &pr, c_locale);
#else
    static locale_t c_locale = newlocale(LC_ALL_MASK, "C", NULL);
    x = strtof_l(st.data(), &pr, c_locale);
#endif
    if (pr == st.data()) {
      throw std::runtime_error("bug in findmax_strtod");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#if defined(_MSC_VER)
#define FROM_CHARS_AVAILABLE_MAYBE
#endif

#ifdef FROM_CHARS_AVAILABLE_MAYBE
double findmax_from_chars(std::vector<std::string> &s) {
  float answer = 0;
  float x = 0;
  for (std::string &st : s) {
    auto [p, ec] = std::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data()) {
      throw std::runtime_error("bug in findmax_from_chars");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#endif

#ifdef ENABLE_RYU
double findmax_ryus2f(std::vector<std::string> &s) {
  float answer = 0;
  float x = 0;
  for (std::string &st : s) {
    // Ryu does not track character consumption (boo), but we can at least...
    Status stat = s2f(st.data(), &x);
    if (stat != SUCCESS) {
      throw std::runtime_error(std::string("bug in findmax_ryus2f ")+st + " " + std::to_string(stat));
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#endif

double findmax_fastfloat(std::vector<std::string> &s) {
  float answer = 0;
  float x = 0;
  for (std::string &st : s) {
    auto [p, ec] = fast_float::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data()) {
      throw std::runtime_error("bug in findmax_fastfloat");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}

double findmax_absl_from_chars(std::vector<std::string> &s) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    auto [p, ec] = absl::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data()) {
      throw std::runtime_error("bug in findmax_absl_from_chars");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#ifdef __linux__
template <class T>
std::vector<event_count> time_it_ns(std::vector<std::string> &lines,
                                     T const &function, size_t repeat) {
  std::vector<event_count> aggregate;
  event_collector collector;
  bool printed_bug = false;
  for (size_t i = 0; i < repeat; i++) {
    collector.start();
    float ts = function(lines);
    if (ts == 0 && !printed_bug) {
      printf("bug\n");
      printed_bug = true;
    }
    aggregate.push_back(collector.end());
 }
  return aggregate;
}

void pretty_print(double volume, size_t number_of_floats, std::string name, std::vector<event_count> events) {
  double volumeMB = volume / (1024. * 1024.);
  double average_ns{0};
  double min_ns{DBL_MAX};
  double cycles_min{DBL_MAX};
  double instructions_min{DBL_MAX};
  double branch_misses_min{DBL_MAX};
  double cycles_avg{0};
  double instructions_avg{0};
  double branch_misses_avg{0};
  for(event_count e : events) {
    double ns = e.elapsed_ns();
    average_ns += ns;
    min_ns = min_ns < ns ? min_ns : ns;

    double cycles = e.cycles();
    cycles_avg += cycles;
    cycles_min = cycles_min < cycles ? cycles_min : cycles;

    double instructions = e.instructions();
    instructions_avg += instructions;
    instructions_min = instructions_min < instructions ? instructions_min : instructions;

    double branch_misses = e.branch_misses();
    branch_misses_avg += branch_misses;
    branch_misses_min = branch_misses_min < branch_misses ? branch_misses_min : branch_misses;
  }
  cycles_avg /= events.size();
  instructions_avg /= events.size();
  branch_misses_avg /= events.size();
  average_ns /= events.size();
  printf("%-40s: %8.2f MB/s (+/- %.1f %%) ", name.data(),
           volumeMB * 1000000000 / min_ns,
           (average_ns - min_ns) * 100.0 / average_ns);
  printf("%8.2f Mfloat/s  ", 
           number_of_floats * 1000 / min_ns);
  if(instructions_min > 0) {
    printf(" %8.2f i/B %8.2f i/f (+/- %.1f %%) ", 
           instructions_min / volume,
           instructions_min / number_of_floats, 
           (instructions_avg - instructions_min) * 100.0 / instructions_avg);
    printf(" %8.2f bm/B %8.2f bm/f (+/- %.1f %%) ", 
           branch_misses_min / volume,
           branch_misses_min / number_of_floats, 
           (branch_misses_avg - branch_misses_min) * 100.0 / branch_misses_avg);

    printf(" %8.2f c/B %8.2f c/f (+/- %.1f %%) ", 
           cycles_min / volume,
           cycles_min / number_of_floats, 
           (cycles_avg - cycles_min) * 100.0 / cycles_avg);
    printf(" %8.2f i/c ", 
           instructions_min /cycles_min);
    printf(" %8.2f GHz ", 
           cycles_min / min_ns);
  }
  printf("\n");

}
#else

template <class T>
std::pair<double, double> time_it_ns(std::vector<std::string> &lines,
                                     T const &function, size_t repeat) {
  std::chrono::high_resolution_clock::time_point t1, t2;
  double average = 0;
  double min_value = DBL_MAX;
  bool printed_bug = false;
  for (size_t i = 0; i < repeat; i++) {
    t1 = std::chrono::high_resolution_clock::now();
    double ts = function(lines);
    if (ts == 0 && !printed_bug) {
      printf("bug\n");
      printed_bug = true;
    }
    t2 = std::chrono::high_resolution_clock::now();
    double dif =
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    average += dif;
    min_value = min_value < dif ? min_value : dif;
  }
  average /= repeat;
  return std::make_pair(min_value, average);
}

void pretty_print(double volume, size_t number_of_floats, std::string name, std::pair<double,double> result) {
  double volumeMB = volume / (1024. * 1024.);
  printf("%-40s: %8.2f MB/s (+/- %.1f %%) ", name.data(),
           volumeMB * 1000000000 / result.first,
           (result.second - result.first) * 100.0 / result.second);
  printf(" %8.2f ns/f \n", 
           double(result.first) /number_of_floats );
}
#endif 
void process(std::vector<std::string> &lines, size_t volume) {
  size_t repeat = 100;
  double volumeMB = volume / (1024. * 1024.);
  std::cout << "volume = " << volumeMB << " MB " << std::endl;
  pretty_print(volume, lines.size(), "strtof", time_it_ns(lines, findmax_strtof, repeat));
#if 0 && defined(ENABLE_RYU)
  // Ryu finds the input too long. Not odd, since it's random doubles...
  pretty_print(volume, lines.size(), "ryu_parse", time_it_ns(lines, findmax_ryus2f, repeat));
#endif
  pretty_print(volume, lines.size(), "abseil", time_it_ns(lines, findmax_absl_from_chars, repeat));
  pretty_print(volume, lines.size(), "fastfloat", time_it_ns(lines, findmax_fastfloat, repeat));
#ifdef FROM_CHARS_AVAILABLE_MAYBE
  pretty_print(volume, lines.size(), "from_chars", time_it_ns(lines, findmax_from_chars, repeat));
#endif
}

void fileload(const char *filename) {
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
  std::cout << "# read " << lines.size() << " lines " << std::endl;
  process(lines, volume);
}

void parse_random_numbers(size_t howmany, bool concise, std::string random_model) {
  std::cout << "# parsing random numbers" << std::endl;
  std::vector<std::string> lines;
  auto g = std::unique_ptr<string_number_generator>(get_generator_by_name(random_model));
  std::cout << "model: " << g->describe() << std::endl;
  if(concise) { std::cout << "concise (using as few digits as possible)"  << std::endl; }
  std::cout << "volume: "<< howmany << " floats"  << std::endl;
  lines.reserve(howmany); // let us reserve plenty of memory.
  size_t volume = 0;
  for (size_t i = 0; i < howmany; i++) {
    std::string line =  g->new_string(concise);
    volume += line.size();
    lines.push_back(line);
  }
  process(lines, volume);
}

cxxopts::Options
    options("benchmark",
            "Compute the parsing speed of different number parsers.");

int main(int argc, char **argv) {
  try {
    options.add_options()
        ("c,concise", "Concise random floating-point strings (if not 17 digits are used)")
        ("f,file", "File name.", cxxopts::value<std::string>()->default_value(""))
        ("v,volume", "Volume (number of floats generated).", cxxopts::value<size_t>()->default_value("100000"))
        ("m,model", "Random Model.", cxxopts::value<std::string>()->default_value("uniform"))
        ("h,help","Print usage.");
    auto result = options.parse(argc, argv);
    if(result["help"].as<bool>()) {
      std::cout << options.help() << std::endl;
      return EXIT_SUCCESS;
    }
    if (result["file"].as<std::string>().empty()) {
      parse_random_numbers(result["volume"].as<size_t>(), result["concise"].as<bool>(), result["model"].as<std::string>());
      std::cout << "# You can also provide a filename (with the -f flag): it should contain one "
                   "string per line corresponding to a number"
                << std::endl;
    } else {
      fileload(result["file"].as<std::string>().c_str());
    }
  } catch (const cxxopts::OptionException &e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
