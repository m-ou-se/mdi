#include "bigint_impl.h"

struct bigint *bigint_add(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
) {
	if ((a->ssize < 0) == (bssize < 0)) {
		// Same signs, so add.
		if (mdi_add(a->digits, ABS(a->ssize), a->digits, ABS(a->ssize), b, ABS(bssize))) {
			// Carry
			a = bigint_resize(a, a->ssize + (a->ssize < 0 ? -1 : 1));
			a->digits[ABS(a->ssize) - 1] = 1;
		}
	} else {
		// Different signs, so subtract.
		sdigit_t rssize = mdi_sub(a->digits, ABS(a->ssize), a->digits, ABS(a->ssize), b, ABS(bssize));
		a->ssize = a->ssize < 0 ? -rssize : rssize;
	}
	return a;
}
