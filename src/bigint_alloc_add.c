#include "bigint_impl.h"

struct bigint *bigint_alloc_add(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	sdigit_t const an = ABS(assize);
	sdigit_t const bn = ABS(bssize);
	struct bigint *r;
	if ((assize < 0) == (bssize < 0)) {
		// Same signs, so add.
		bool const potential_carry =
			an == bn && (a[an - 1] + b[an - 1] + 1 <= b[an - 1]);
		sdigit_t rn = MAX(an, bn) + potential_carry;
		r = bigint_alloc(assize < 0 ? -rn : rn);
		mdi_add(r->digits, rn, a, an, b, bn);
		if (r->digits[rn - 1] == 0) {
			// Strip leading zero.
			r->ssize -= r->ssize < 0 ? -1 : 1;
		}
	} else {
		// Different signs, so subtract.
		sdigit_t rssize = mdi_sub(NULL, 0, a, an, b, bn);
		if (assize < 0) rssize = -rssize;
		r = bigint_alloc(rssize);
		if (r->ssize) mdi_sub(r->digits, ABS(r->ssize), a, an, b, bn);
	}
	return r;
}
