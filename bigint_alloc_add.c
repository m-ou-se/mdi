#include "bigint_impl.h"

struct bigint *bigint_alloc_add(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	struct bigint *r;
	if ((assize < 0) == (bssize < 0)) {
		// Same signs, so add.
		sdigit_t rn = MAX(ABS(assize), ABS(bssize)) + 1;
		r = bigint_alloc(assize < 0 ? -rn : rn);
		mdi_add(r->digits, rn, a, ABS(assize), b, ABS(bssize));
		if (r->digits[rn - 1] == 0) {
			// Strip leading zero.
			r->ssize -= r->ssize < 0 ? -1 : 1;
		}
	} else {
		// Different signs, so subtract.
		sdigit_t rssize = mdi_sub(NULL, 0, a, ABS(assize), b, ABS(bssize));
		if (assize < 0) rssize = -rssize;
		r = bigint_alloc(rssize);
		if (r->ssize) mdi_sub(r->digits, ABS(r->ssize), a, ABS(assize), b, ABS(bssize));
	}
	return r;
}
