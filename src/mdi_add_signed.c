#include <assert.h>

#include "mdi_impl.h"

bool mdi_add_signed(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn,
	bool negate_b
) {
	assert(rn >= an);
	assert(rn >= bn);
	assert(r == a || !(r < a + an && a < r + rn));
	assert(r == b || !(r < b + bn && b < r + rn));

	digit_t a_extend = an && a[an-1] >> (DIGIT_BITS - 1) ? -1 : 0;
	digit_t b_extend = bn && b[bn-1] >> (DIGIT_BITS - 1) ? -1 : 0;
	int carry = negate_b;
	digit_t ad, bd, rd;
	for (size_t i = 0; i < rn; ++i) {
		ad = i < an ? a[i] : a_extend;
		bd = i < bn ? b[i] : b_extend;
		if (negate_b) bd = ~bd;
		carry = add_overflow(ad, carry, &rd);
		carry |= add_overflow(rd, bd, &rd);
		r[i] = rd;
	}
	return rn && ((~ad ^ bd) & (ad ^ rd)) >> (DIGIT_BITS - 1);
}

