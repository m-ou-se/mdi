#include "bigint_impl.h"

struct bigint *bigint_alloc_mul(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	if (assize == 0 || bssize == 0) return bigint_alloc(0);
	sdigit_t rn = ABS(assize) + ABS(bssize);
	struct bigint *r = bigint_alloc(rn);
	mdi_mul(r->digits, rn, a, ABS(assize), b, ABS(bssize));
	while (rn--) if (r->digits[rn] != 0) break;
	++rn;
	r->ssize = (assize < 0) != (bssize < 0) ? -rn : rn;
	return r;
}
