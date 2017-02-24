#include "bigint_impl.h"

static void strip_leading_zeros(struct bigint *b) {
	size_t n = ABS(b->ssize);
	while (n && b->digits[n - 1] == 0) --n;
	b->ssize = b->ssize < 0 ? -(sdigit_t)n : n;
}

struct bigint *bigint_alloc_add(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	struct bigint *r;
	if ((assize < 0) == (bssize < 0)) {
		// Same signs, so add.
		r = bigint_alloc(assize + (assize < 0 ? -1 : 1));
		mdi_add(r->digits, ABS(r->ssize), a, ABS(assize), b, ABS(bssize));
		strip_leading_zeros(r);
	} else {
		// Different signs, so subtract.
		sdigit_t rssize = mdi_sub(NULL, 0, a, ABS(assize), b, ABS(bssize));
		if (assize < 0) rssize = -rssize;
		r = bigint_alloc(rssize);
		if (r->ssize) mdi_sub(r->digits, ABS(r->ssize), a, ABS(assize), b, ABS(bssize));
	}
	return r;
}
