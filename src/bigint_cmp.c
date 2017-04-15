#include "bigint_impl.h"

int bigint_cmp(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	if (assize > bssize) return 1;
	if (assize < bssize) return -1;

	for (size_t n = ABS(assize); n--;) {
		if (a[n] > b[n]) return assize < 0 ? -1 : 1;
		if (a[n] < b[n]) return assize < 0 ? 1 : -1;
	}

	return 0;
}
