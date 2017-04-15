#include <assert.h>

#include "bigint_impl.h"

struct bigint *bigint_add(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
) {
	sdigit_t const an = ABS(a->ssize);
	sdigit_t const bn = ABS(bssize);
	if ((a->ssize < 0) == (bssize < 0)) {
		// Same signs, so add.
		bool const potential_carry =
			an == bn && (a->digits[an - 1] + b[an - 1] + 1 <= b[an - 1]);
		sdigit_t const rn = MAX(an, bn) + potential_carry;
		if (bn > an) a = bigint_resize(a, a->ssize < 0 ? -rn : rn);
		if (mdi_add(a->digits, ABS(a->ssize), a->digits, an, b, bn)) {
			// Carry
			assert(potential_carry);
			assert(rn == an + 1);
			a = bigint_resize(a, rn);
			a->digits[rn - 1] = 1;
		} else if (ABS(a->ssize) == rn) {
			// No carry, potentially allocated one digit too many.
			if (a->digits[rn - 1] == 0) {
				// Strip leading zero.
				a->ssize -= a->ssize < 0 ? -1 : 1;
			}
		}
	} else {
		// Different signs, so subtract.
		sdigit_t rssize = mdi_sub(NULL, 0, a->digits, an, b, bn);
		if (a->ssize < 0) rssize = -rssize;
		a = bigint_resize(a, rssize);
		if (rssize != 0) mdi_sub(a->digits, ABS(a->ssize), a->digits, an, b, bn);
	}
	return a;
}
