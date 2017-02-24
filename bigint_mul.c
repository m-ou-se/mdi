#include <assert.h>

#include "bigint_impl.h"

struct bigint *bigint_mul(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
) {
	if (a->ssize == 0) return a;
	if (bssize == 0) return bigint_resize(a, 0);

	size_t const rn = ABS(a->ssize) + ABS(bssize);

	// If bssize is only 1, it might not overflow.
	if (ABS(bssize) != 1) a = bigint_resize(a, rn);

	digit_t overflow = mdi_mul(a->digits, rn, a->digits, ABS(a->ssize), b, ABS(bssize));

	if (overflow) {
		assert(ABS(bssize) == 1);
		a = bigint_resize(a, rn);
		a->digits[rn - 1] = overflow;
	}

	if (bssize < 0) a->ssize = -a->ssize;

	return a;
}
