#include "bigint_impl.h"

struct bigint *bigint_alloc_mul(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	if (assize == 0 || bssize == 0) return bigint_alloc(0);
	size_t rn = ABS(assize) + ABS(bssize);
	struct bigint *r = bigint_alloc((assize < 0) != (bssize < 0) ? -(sdigit_t)rn : rn);
	mdi_mul(r->digits, rn, a, ABS(assize), b, ABS(bssize));
	return r;
}
