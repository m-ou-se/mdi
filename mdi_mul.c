#include <assert.h>

#include "mdi_impl.h"

digit_t mdi_mul(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	// Quadratic time school-book multiplication.
	assert(rn >= an + bn - 1);
	assert(r == a || !(r < a + an && a < r + rn));
	assert(!(r < b + bn && b < r + rn));
	digit_t overflow = 0;
	for (size_t i = an; i < rn; ++i) r[i] = 0;
	for (size_t i = an; i--;) {
		digit_t carry = 0;
		digit_t ad = a[i];
		r[i] = 0;
		for (size_t j = 0; j < bn; ++j) {
			digit_t low, high;
			multiply_extend(ad, b[j], &low, &high);
			size_t k = i + j;
			bool c1 = add_overflow(carry, r[k], &r[k]);
			bool c2 = add_overflow(low, r[k], &r[k]);
			assert(!(c1 && c2));
			carry = high + (c1 || c2);
		}
		for (size_t k = i + bn; k < rn && carry; ++k) {
			carry = add_overflow(carry, r[k], &r[k]);
		}
		overflow += carry;
	}
	return overflow;
}
