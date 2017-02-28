#include <assert.h>

#include "mdi_impl.h"

inline static ddigit_t ddigit(digit_t high, ddigit_t low) {
	return (ddigit_t)high << DIGIT_BITS | (ddigit_t)low;
}

void mdi_div(
	digit_t *q,
	digit_t *u, size_t un,
	digit_t const *v, size_t vn
) {
	// Can't divide by zero.
	assert(vn > 0);

	// Don't allow leading zeros.
	assert(v[vn - 1] > 0);

	if (un < vn) {
		// q is 0. Its size is 0, so no digits to write.
		// Remainder is u and returned in u, so nothing to do for that either.
		return;
	}

	// v may not overlap with neither u nor q.
	assert(!(v < u + un && u < v + vn));
	assert(!(v < q + un - vn + 1 && q < v + vn));

	// q may only partially overlap with u.
	assert(!(q <= u + vn && u < q + un - vn + 1));

	if (vn == 1) {
		u[0] = mdi_div1(q, u, un, v[0]);
		return;
	}

	// Algorithm based on algorithm D from section 4.3.1 of The Art of Computer
	// Programming Volume 2 by Donald Knuth.

	int const lshift = num_leading_zeros(v[vn - 1]);

	digit_t v1 = v[vn - 1];
	digit_t v2 = v[vn - 2];

	if (lshift) {
		digit_t v3 = vn >= 3 ? v[vn - 3] : 0;
		v1 = v1 << lshift | v2 >> (DIGIT_BITS - lshift);
		v2 = v2 << lshift | v3 >> (DIGIT_BITS - lshift);
	}

	for (size_t j = un; j >= vn; --j) {

		ddigit_t u01 = (j < un ? (ddigit_t)u[j] << DIGIT_BITS : 0) | u[j - 1];
		digit_t u2 = u[j - 2];

		if (lshift) {
			digit_t u3 = j >= 3 ? u[j - 3] : 0;
			u01 = u01 << lshift | u2 >> (DIGIT_BITS - lshift);
			u2  = u2  << lshift | u3 >> (DIGIT_BITS - lshift);
		}

		digit_t qd;
		ddigit_t r;

		// Guess the digit of q at this position.
		if (u01 >> DIGIT_BITS == v1) {
			qd = (digit_t)-1;
			r = u01 - (ddigit_t)v1 * qd;
		} else {
			qd = u01 / v1;
			r  = u01 % v1;
		}

		// Improve guess.
		while (qd && r <= DIGIT_MAX && (ddigit_t)v2 * qd > ((ddigit_t)r << DIGIT_BITS | u2)) {
			--qd;
			r = u01 - (ddigit_t)v1 * qd;
		}

		// Subtract qd times v at this position.
		digit_t borrow = 0;
		for (size_t i = 0; i < vn; ++i) {
			digit_t low, high;
			multiply_extend(qd, v[i], &low, &high);
			digit_t x = u[j - vn + i];
			high += sub_overflow(x, low, &x);
			high += sub_overflow(x, borrow, &x);
			u[j - vn + i] = x;
			borrow = high;
		}
		digit_t x = j < un ? u[j] : 0;
		borrow = sub_overflow(x, borrow, &x);
		assert(borrow ? x == DIGIT_MAX : x == 0);

		if (borrow) {
			// qd was one too high.
			--qd;
			// Add one v back to correct.
			digit_t carry = 0;
			for (size_t i = 0; i < vn; ++i) {
				digit_t x = u[j - vn + i];
				carry = add_overflow(x, carry, &x);
				carry += add_overflow(x, v[i], &x);
				u[j - vn + i] = x;
			}
			assert(carry); // Carry cancels out the borrow.
		}

		q[j - vn] = qd;
	}
}
