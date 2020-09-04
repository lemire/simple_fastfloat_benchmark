#ifndef FASTFLOAT_DECIMAL_TO_BINARY_H
#define FASTFLOAT_DECIMAL_TO_BINARY_H

#include "float_common.h"
#include "fast_table.h"
#include "tables_negative.h"
#include "tables_positive.h"
#include <cfloat>
#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace fastfloat {




// will compute w * 5**q
value128 complete_long_product(int64_t q, uint64_t w) {
  uint64_t fullanswer[14];

  bool positive_power = (q >= 0);
  const uint64_t *p5 =
      positive_power ? powers_of_five : negative_powers_of_five;
  typedef uint16_t pair_of_uint16[2];
  const pair_of_uint16 *index5 = positive_power
                                     ? index_into_powers_of_five
                                     : negative_index_into_powers_of_five;
  q = positive_power ? q : -q;
  value128 firstproduct = full_multiplication(w, p5[q]);
  // rewrite to avoid the stores
  fullanswer[0] = firstproduct.high;
  fullanswer[1] = firstproduct.low;

  int level = 0;
  if (firstproduct.low + w - 1 < firstproduct.low) {
    int smallest_power;
    level++; // start at level one, meaning the second word
    // here we must have that q<309.
    for (; q >= (smallest_power = index5[level][0]); level++) {
      const int64_t idx = index5[level][1] + (q - smallest_power);
      value128 product = full_multiplication(w, p5[idx]);
      fullanswer[level + 1] = product.low;
      fullanswer[level] += product.high;      // can overflow
      if (product.high > fullanswer[level]) { // we have an overflow
        int tmpidx = level - 1;
        fullanswer[tmpidx]++;
        while (fullanswer[tmpidx] ==
               0) { // if we have an overflow, we need to go downw further
          tmpidx--;
          fullanswer[tmpidx]++;
        }
      }
      // we only need to go further if we have all 1s.
      if (fullanswer[level] != 0xFFFFFFFFFFFFFFFF) {
        break;
      }
    }
  }
  value128 answer(fullanswer[1], fullanswer[0]);
  return answer;
}


// will compute w * 5**q
fastfloat_really_inline
value128 compute_product(int64_t q, uint64_t w) {
  value128 firstproduct = full_multiplication(w, power_of_five_128[2 * (q + 344)]);
  value128 secondproduct = full_multiplication(w, power_of_five_128[2 * (q + 344) + 1]);
  firstproduct.low += secondproduct.high;
  if(secondproduct.high > firstproduct.low) {
    firstproduct.high++;
  }
  // At this point, we might need to add at most one to firstproduct, but this
  // can only change the value of firstproduct.high if firstproduct.low is maximal.
  if(firstproduct.low  == 0xFFFFFFFFFFFFFFFF) {
    // This is very unlikely, but if so, we need to do much more work!
    return complete_long_product(q,w);
  }
  return firstproduct;
}


namespace {
/**
 * For q in (-400,350), we have that
 *  f = (((152170 + 65536) * q ) >> 16);
 * is equal to
 *   floor(p) + q
 * where
 *   p = log(5**q)/log(2) = q * log(5)/log(2)
 *
 */
fastfloat_really_inline unsigned int power(int q)  noexcept  {
  return (((152170 + 65536) * q) >> 16) + 63;
}
} // namespace

// w * 10 ** q
template <typename binary>
fastfloat_really_inline
 adjusted_mantissa compute_float(int64_t q, uint64_t w)  noexcept  {
  adjusted_mantissa answer;
  if (w == 0) {
    answer.power2 = 0;
    answer.mantissa = 0;
    return answer;
  }
  if (q > 308) {
    // we want to get infinity:
    answer.power2 = binary::infinite_power();
    answer.mantissa = 0;
    return answer;
  } else if (q < -324 - 19) {
    // should be zero
    answer.power2 = 0;
    answer.mantissa = 0;
    return answer;
  }

  // We want the most significant bit of i to be 1. Shift if needed.
  int lz = leading_zeroes(w);
  w <<= lz;

  value128 product = compute_product(q, w);
  uint64_t upperbit = product.high >> 63;
  answer.mantissa = product.high >> (upperbit + 64 - binary::mantissa_explicit_bits() - 3);
  lz += int(1 ^ upperbit);
  answer.power2 = power(int(q)) - lz - binary::minimum_exponent() + 1;

  if (answer.power2 <= 0) { // we have a subnormal?
    answer.mantissa >>= -answer.power2 + 1;
    answer.mantissa += (answer.mantissa & 1); // round up
    answer.mantissa >>= 1;
    answer.power2 = (answer.mantissa < (uint64_t(1) << binary::mantissa_explicit_bits())) ? 0 : 1;
    return answer;
  }
  // usually, we round *up*, but if we fall right in between and and we have an
  // even basis, we need to round down
  uint64_t mask = (uint64_t(2) << (64 - binary::mantissa_explicit_bits() - 1))-1;
  if ((product.low == 0) && (q >= 0) && (q <= binary::max_power_for_even()) &&
      ((product.high & mask) == 0) &&
      ((answer.mantissa & 3) == 1)) { // we may fall between two floats!
    answer.mantissa ^= 1;             // flip it so that we do not round up
  }
  answer.mantissa += (answer.mantissa & 1); // round up
  answer.mantissa >>= 1;

  if (answer.mantissa >= (uint64_t(2) << binary::mantissa_explicit_bits())) {
    answer.mantissa = (uint64_t(1) << binary::mantissa_explicit_bits());
    answer.power2++; // undo previous addition
  }

  answer.mantissa &= ~(uint64_t(1) << binary::mantissa_explicit_bits());
  if (answer.power2 >= binary::infinite_power()) { // infinity
    answer.power2 = binary::infinite_power();
    answer.mantissa = 0;
  }
  return answer;
}


} // namespace fastfloat

#endif
