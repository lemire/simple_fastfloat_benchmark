#ifndef FASTFLOAT_RTN_H
#define FASTFLOAT_RTN_H
// fast_float compiled with FASTFLOAT_ROUNDS_TO_NEAREST (see fastfloat_rtn.cpp).
#include <string>
#include <vector>

template <typename CharT>
double findmax_fastfloat_rtn(std::vector<std::basic_string<CharT>> &s,
                             bool expect_error);
template <typename CharT>
double findmax_fastfloat32_rtn(std::vector<std::basic_string<CharT>> &s);

#endif
