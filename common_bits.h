#include "mp.h"

#define add_overflow __builtin_add_overflow

#define sub_overflow __builtin_sub_overflow

inline static void multiply_extend(digit_t a, digit_t b, digit_t *low, digit_t *high) {
	ddigit_t result = (ddigit_t)a * (ddigit_t)b;
	*low = result & DIGIT_MAX;
	*high = result >> DIGIT_BITS;
}
