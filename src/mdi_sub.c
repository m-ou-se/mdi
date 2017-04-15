#include <assert.h>

#include "mdi_impl.h"

sdigit_t mdi_sub(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	assert(r == a || !(r < a + an && a < r + rn));
	assert(r == b || !(r < b + bn && b < r + rn));

	// The number of digits we'll consider.
	size_t n = rn;
	if (r == NULL && n == 0) n = an > bn ? an : bn;

	// Find the common prefix and check which one is larger.
	bool a_lt_b;
	while (n--) {
		digit_t ad = n < an ? a[n] : 0;
		digit_t bd = n < bn ? b[n] : 0;
		if (ad < bd) { a_lt_b = true; break; }
		if (ad > bd) { a_lt_b = false; break; }
	}
	++n;

	if (r != NULL && rn > 0) {

		// Fill leading zero's for r.
		for (size_t i = n; i < rn; ++i) r[i] = 0;

		// Swap a and b if a < b, to make sure a > b.
		if (a_lt_b) {
			digit_t const *t = a; a = b; b = t;
			size_t tn = an; an = bn; bn = tn;
		}

		// Fill the rest of r with a - b.
		digit_t borrow = 0;
		if (n > 0 && bn == 1) {
			// Optimized version when b is single-digit.
			borrow = b[0];
			for (size_t i = 0; i < n; ++i) {
				digit_t ad = i < an ? a[i] : 0;
				borrow = sub_overflow(ad, borrow, &r[i]);
			}
		} else {
			for (size_t i = 0; i < n; ++i) {
				digit_t ad = i < an ? a[i] : 0;
				digit_t bd = i < bn ? b[i] : 0;
				digit_t rd;
				borrow = sub_overflow(ad, borrow, &rd);
				borrow |= sub_overflow(rd, bd, &rd);
				r[i] = rd;
			}
		}
		assert(borrow == 0);
	}

	return a_lt_b ? -(sdigit_t)n : (sdigit_t)n;
}
