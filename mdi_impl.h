#include "mdi.h"

#if DIGIT_BITS == 64
typedef __uint128_t ddigit_t;
#elif DIGIT_BITS == 32
typedef uint64_t ddigit_t;
#else
#error DIGIT_BITS is neither 64 nor 32.
#endif

#define add_overflow __builtin_add_overflow

#define sub_overflow __builtin_sub_overflow

inline static void multiply_extend(digit_t a, digit_t b, digit_t *low, digit_t *high) {
	ddigit_t result = (ddigit_t)a * (ddigit_t)b;
	*low = result & DIGIT_MAX;
	*high = result >> DIGIT_BITS;
}

#define num_leading_zeros __builtin_clzll
