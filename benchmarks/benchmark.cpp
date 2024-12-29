#ifdef __CYGWIN__
#define _GNU_SOURCE // for strtod_l
#endif

#ifndef __CYGWIN__
#include "absl/strings/charconv.h"
#include "absl/strings/numbers.h"
#endif
#include "fast_float/fast_float.h"

#ifdef ENABLE_RYU
#include "ryu_parse.h"
#endif


#include "double-conversion/ieee.h"
#include "double-conversion/double-conversion.h"

#define IEEE_8087
#include "cxxopts.hpp"
#if defined(__linux__) || (__APPLE__ &&  __aarch64__)
#define USING_COUNTERS
#include "event_counter.h"
#endif
#include "dtoa.c"
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
#if !defined(_MSC_VER) && !defined(__CYGWIN__) // assume that everything that is not GLIBC, Cygwin or Visual
                                               // Studio needs xlocale.h
#include <xlocale.h>
#endif
#endif

template <typename CharT>
double findmax_doubleconversion(std::vector<std::basic_string<CharT>> &s, bool expect_error) {
  double answer = 0;
  double x;
  // from_chars does not allow leading spaces:
  // double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES |
  int flags = double_conversion::StringToDoubleConverter::ALLOW_TRAILING_JUNK |
              double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES;
  double empty_string_value = 0.0;
  uc16 separator = double_conversion::StringToDoubleConverter::kNoSeparator;
  double_conversion::StringToDoubleConverter converter(
      flags, empty_string_value, double_conversion::Double::NaN(), NULL, NULL,
      separator);
  int processed_characters_count;
  for (auto &st : s) {
    if constexpr (std::is_same<CharT, char16_t>::value) {
      x = converter.StringToDouble((const uc16*)st.data(), st.size(), &processed_characters_count);
    } else { 
      x = converter.StringToDouble(st.data(), st.size(), &processed_characters_count);
    }
    if (processed_characters_count == 0 && expect_error == false) {
      throw std::runtime_error("bug in findmax_doubleconversion");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#ifdef _WIN32
double findmax_strtod_16(std::vector<std::u16string>& s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (auto& st : s) {
    auto* pr = (wchar_t*)st.data();
    static _locale_t c_locale = _create_locale(LC_ALL, "C");
    x = _wcstod_l((const wchar_t *)st.data(), &pr, c_locale);

    if (pr == (const wchar_t*)st.data() && expect_error == false) {
      throw std::runtime_error("bug in findmax_strtod");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#endif
double findmax_netlib(std::vector<std::string> &s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    char *pr = (char *)st.data();
    x = netlib_strtod(st.data(), &pr);
    if (pr == st.data() && expect_error == false) {
      throw std::runtime_error(std::string("bug in findmax_netlib ")+st);
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}

#ifdef ENABLE_RYU
double findmax_ryus2d(std::vector<std::string> &s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    // Ryu does not track character consumption (boo), but we can at least...
    Status stat = s2d(st.data(), &x);
    if (stat != SUCCESS && expect_error == false) {
      throw std::runtime_error(std::string("bug in findmax_ryus2d ")+st + " " + std::to_string(stat));
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#endif

double findmax_strtod(std::vector<std::string> &s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    char *pr = (char *)st.data();
#ifdef _WIN32
    static _locale_t c_locale = _create_locale(LC_ALL, "C");
    x = _strtod_l(st.data(), &pr, c_locale);
#else
    static locale_t c_locale = newlocale(LC_ALL_MASK, "C", NULL);
    x = strtod_l(st.data(), &pr, c_locale);
#endif
    if (pr == st.data() && expect_error == false) {
      throw std::runtime_error("bug in findmax_strtod");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
// Why not `|| __cplusplus > 201703L`? Because GNU libstdc++ does not have
// float parsing for std::from_chars.
#if defined(_MSC_VER)
#define FROM_CHARS_AVAILABLE_MAYBE
#endif

#ifdef FROM_CHARS_AVAILABLE_MAYBE
double findmax_from_chars(std::vector<std::string> &s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    auto [p, ec] = std::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data() && expect_error == false) {
      throw std::runtime_error("bug in findmax_from_chars");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#endif

template <typename CharT>
double findmax_fastfloat(std::vector<std::basic_string<CharT>> &s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (auto &st : s) {
    auto [p, ec] = fast_float::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data() && expect_error == false) {
      throw std::runtime_error("bug in findmax_fastfloat");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}

#ifndef __CYGWIN__
double findmax_absl_from_chars(std::vector<std::string> &s, bool expect_error) {
  double answer = 0;
  double x = 0;
  for (std::string &st : s) {
    auto [p, ec] = absl::from_chars(st.data(), st.data() + st.size(), x);
    if (p == st.data() && expect_error == false) {
      throw std::runtime_error("bug in findmax_absl_from_chars");
    }
    answer = answer > x ? answer : x;
  }
  return answer;
}
#endif

#ifdef USING_COUNTERS
template <class T, class CharT>
std::vector<event_count> time_it_ns(std::vector<std::basic_string<CharT>> &lines,
                                     T const &function, size_t repeat, bool expect_error) {
  std::vector<event_count> aggregate;
  event_collector collector;
  bool printed_bug = false;
  for (size_t i = 0; i < repeat; i++) {
    collector.start();
    double ts = function(lines,expect_error);
    if (ts == 0 && !printed_bug && expect_error == false) {
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
  double cycles_avg{0};
  double instructions_avg{0};
  double branches_min{0};
  double branches_avg{0};
  double branch_misses_min{0};
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

    double branches = e.branches();
    branches_avg += branches;
    branches_min = branches_min < branches ? branches_min : branches;

    double branch_misses = e.missed_branches();
    branch_misses_avg += branch_misses;
    branch_misses_min = branch_misses_min < branch_misses ? branch_misses_min : branch_misses;
  }
  cycles_avg /= events.size();
  instructions_avg /= events.size();
  average_ns /= events.size();
  branches_avg /= events.size();
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

    printf(" %8.2f c/B %8.2f c/f (+/- %.1f %%) ", 
           cycles_min / volume,
           cycles_min / number_of_floats, 
           (cycles_avg - cycles_min) * 100.0 / cycles_avg);
    printf(" %8.2f i/c ", 
           instructions_min /cycles_min);
    printf(" %8.2f b/f ",
           branches_avg /number_of_floats);
    printf(" %8.2f bm/f ",
           branch_misses_avg /number_of_floats);
    printf(" %8.2f GHz ", 
           cycles_min / min_ns);
  }
  printf("\n");

}
#else
template <class T, class CharT>
std::pair<double, double> time_it_ns(std::vector<std::basic_string<CharT>> &lines,
                                     T const &function, size_t repeat, bool expect_error) {
  std::chrono::high_resolution_clock::time_point t1, t2;
  double average = 0;
  double min_value = DBL_MAX;
  bool printed_bug = false;
  for (size_t i = 0; i < repeat; i++) {
    t1 = std::chrono::high_resolution_clock::now();
    double ts = function(lines, expect_error);
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
  printf("%8.2f Mfloat/s  ", 
           number_of_floats * 1000 / result.first);
  printf(" %8.2f ns/f \n", 
           double(result.first) /number_of_floats );
}
#endif 


// this is okay, all chars are ASCII
inline std::u16string widen(std::string line) {
  std::u16string u16line;
  u16line.resize(line.size());
  for (size_t i = 0; i < line.size(); ++i) {
    u16line[i] = char16_t(line[i]);
  }
  return u16line;
}

std::vector<std::u16string> widen(const std::vector<std::string> &lines) {
  std::vector<std::u16string> u16lines;
  u16lines.reserve(lines.size());
  for (auto const &line : lines) {
    u16lines.push_back(widen(line));
  }
  return u16lines;
}


void process(std::vector<std::string> &lines, size_t volume, bool expect_error) {
  size_t repeat = 100;
  double volumeMB = volume / (1024. * 1024.);
  std::cout << "ASCII volume = " << volumeMB << " MB " << std::endl;
  pretty_print(volume, lines.size(), "netlib", time_it_ns(lines, findmax_netlib, repeat, expect_error));
  pretty_print(volume, lines.size(), "doubleconversion", time_it_ns(lines, findmax_doubleconversion<char>, repeat, expect_error));
  pretty_print(volume, lines.size(), "strtod", time_it_ns(lines, findmax_strtod, repeat, expect_error));
#ifdef ENABLE_RYU
  pretty_print(volume, lines.size(), "ryu_parse", time_it_ns(lines, findmax_ryus2d, repeat, expect_error));
#endif
#ifndef __CYGWIN__
  pretty_print(volume, lines.size(), "abseil", time_it_ns(lines, findmax_absl_from_chars, repeat, expect_error));
#endif
  pretty_print(volume, lines.size(), "fastfloat", time_it_ns(lines, findmax_fastfloat<char>, repeat, expect_error));
#ifdef FROM_CHARS_AVAILABLE_MAYBE
  pretty_print(volume, lines.size(), "from_chars", time_it_ns(lines, findmax_from_chars, repeat, expect_error));
#endif
  std::vector<std::u16string> lines16 = widen(lines);
  volume = 2 * volume;
  volumeMB = volume / (1024. * 1024.);
  std::cout << "UTF-16 volume = " << volumeMB << " MB " << std::endl;
  pretty_print(volume, lines.size(), "doubleconversion", time_it_ns(lines16, findmax_doubleconversion<char16_t>, repeat, expect_error));
#ifdef _WIN32
  pretty_print(volume, lines.size(), "wcstod", time_it_ns(lines16, findmax_strtod_16, repeat, expect_error));
#endif
  pretty_print(volume, lines.size(), "fastfloat", time_it_ns(lines16, findmax_fastfloat<char16_t>, repeat, expect_error));
}

void fileload(const char *filename, bool expect_error) {
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
  process(lines, volume, expect_error);
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
  process(lines, volume, false);
}

void parse_contrived(size_t howmany, const char *filename) {
  std::cout << "# parsing contrived numbers" << std::endl;
  std::cout << "# these are contrived test cases to test specific algorithms" << std::endl;
  std::vector<std::string> lines;
  std::cout << "volume: "<< howmany << " floats"  << std::endl;

  std::ifstream inputfile(filename);
  if (!inputfile) {
    std::cerr << "can't open " << filename << std::endl;
    return;
  }
  lines.reserve(howmany); // let us reserve plenty of memory.
  std::string line;
  while (getline(inputfile, line)) {
    std::cout << "testing contrived case: \"" << line << "\"" << std::endl;
    size_t volume = 0;
    for (size_t i = 0; i < howmany; i++) {
      lines.push_back(line);
    }
    process(lines, volume, false);
    lines.clear();
    std::cout << "-----------------------" << std::endl;
  }
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
        ("e,errors", "Enable testing of invalid inputs. inputs should come from -f/--file option.")
        ("h,help","Print usage.");
    auto result = options.parse(argc, argv);
    bool test_errors = result["errors"].as<bool>();
    if(result["help"].as<bool>()) {
      std::cout << options.help() << std::endl;
      return EXIT_SUCCESS;
    }
    auto filename = result["file"].as<std::string>();
    if (filename.find("contrived") != std::string::npos) {
      parse_contrived(result["volume"].as<size_t>(), filename.c_str());
      std::cout << "# You can also provide a filename (with the -f flag): it should contain one "
                   "string per line corresponding to a number"
                << std::endl;
    } else if (filename.empty()) {
      parse_random_numbers(result["volume"].as<size_t>(), result["concise"].as<bool>(), result["model"].as<std::string>());
      std::cout << "# You can also provide a filename (with the -f flag): it should contain one "
                   "string per line corresponding to a number"
                << std::endl;
    } else {
      fileload(filename.c_str(), test_errors);
    }
  } catch (const cxxopts::OptionException &e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
