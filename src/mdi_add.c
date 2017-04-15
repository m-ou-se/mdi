#include <assert.h>

#include "mdi_impl.h"

bool mdi_add(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	assert(r == a || !(r < a + an && a < r + rn));
	assert(r == b || !(r < b + bn && b < r + rn));
	assert(rn >= an);
	assert(rn >= bn);

	digit_t carry = 0;

	if (bn == 1) {
		// Optimized version when b is single-digit.
		carry = b[0];
		for (size_t i = 0; i < rn; ++i) {
			digit_t ad = i < an ? a[i] : 0;
			carry = add_overflow(ad, carry, &r[i]);
		}
		return carry;
	} else {
		for (size_t i = 0; i < rn; ++i) {
			digit_t ad = i < an ? a[i] : 0;
			digit_t bd = i < bn ? b[i] : 0;
			carry = add_overflow(ad, carry, &r[i]);
			carry |= add_overflow(r[i], bd, &r[i]);
		}
	}

	assert(carry == 0 || carry == 1);

	return carry;
}
