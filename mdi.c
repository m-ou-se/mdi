#include <assert.h>

#include "mdi.h"

#include "common_bits.h"

bool mdi_add_signed(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn,
	bool negate_b
) {
	assert(rn >= an);
	assert(rn >= bn);
	assert(r == a || !(r < a + an && a < r + rn));
	assert(r == b || !(r < b + bn && b < r + rn));

	digit_t a_extend = an && a[an-1] >> (DIGIT_BITS - 1) ? -1 : 0;
	digit_t b_extend = bn && b[bn-1] >> (DIGIT_BITS - 1) ? -1 : 0;
	int carry = negate_b;
	digit_t ad, bd, rd;
	for (size_t i = 0; i < rn; ++i) {
		ad = i < an ? a[i] : a_extend;
		bd = i < bn ? b[i] : b_extend;
		if (negate_b) bd = ~bd;
		carry = add_overflow(ad, carry, &rd);
		carry |= add_overflow(rd, bd, &rd);
		r[i] = rd;
	}
	return rn && ((~ad ^ bd) & (ad ^ rd)) >> (DIGIT_BITS - 1);
}

bool mdi_add(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	assert(r == a || !(r < a + an && a < r + rn));
	assert(r == b || !(r < b + bn && b < r + rn));
	assert(rn >= an);
	assert(rn >= bn);
	if (bn == 1) {
		digit_t carry = b[0];
		for (size_t i = 0; i < rn; ++i) {
			digit_t ad = i < an ? a[i] : 0;
			carry = add_overflow(ad, carry, &r[i]);
		}
		return carry;
	} else {
		int carry = 0;
		for (size_t i = 0; i < rn; ++i) {
			digit_t ad = i < an ? a[i] : 0;
			digit_t bd = i < bn ? b[i] : 0;
			carry = add_overflow(ad, carry, &r[i]);
			carry |= add_overflow(r[i], bd, &r[i]);
		}
		return rn && carry;
	}
}

sdigit_t mdi_sub(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	assert(r == a || !(r < a + an && a < r + rn));
	assert(r == b || !(r < b + bn && b < r + rn));

	// The number of digits we'll consider.
	size_t n = rn;
	if (r == NULL && n == 0) n = an > bn ? an : bn;

	// Find the common prefix and check which one is larger.
	bool a_lt_b;
	while (n--) {
		digit_t ad = n < an ? a[n] : 0;
		digit_t bd = n < bn ? b[n] : 0;
		if (ad < bd) { a_lt_b = true; break; }
		if (ad > bd) { a_lt_b = false; break; }
	}
	++n;

	if (r != NULL && rn > 0) {

		// Fill leading zero's for r.
		for (size_t i = n; i < rn; ++i) r[i] = 0;

		// Swap a and b if a < b, to make sure a > b.
		if (a_lt_b) {
			digit_t const *t = a; a = b; b = t;
			size_t tn = an; an = bn; bn = tn;
		}

		// Fill the rest of r with a - b.
		int borrow = 0;
		for (size_t i = 0; i < n; ++i) {
			digit_t ad = i < an ? a[i] : 0;
			digit_t bd = i < bn ? b[i] : 0;
			digit_t rd;
			borrow = sub_overflow(ad, borrow, &rd);
			borrow |= sub_overflow(rd, bd, &rd);
			r[i] = rd;
		}
	}

	return a_lt_b ? -(sdigit_t)n : n;
}

digit_t mdi_mul(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	// Quadratic time school-book multiplication.
	assert(rn >= an + bn - 1);
	assert(r == a || !(r < a + an && a < r + rn));
	assert(!(r < b + bn && b < r + rn));
	digit_t overflow = 0;
	for (size_t i = an; i < rn; ++i) r[i] = 0;
	for (size_t i = an; i--;) {
		digit_t carry = 0;
		digit_t ad = a[i];
		r[i] = 0;
		for (size_t j = 0; j < bn; ++j) {
			digit_t low, high;
			multiply_extend(ad, b[j], &low, &high);
			size_t k = i + j;
			bool c1 = add_overflow(carry, r[k], &r[k]);
			bool c2 = add_overflow(low, r[k], &r[k]);
			assert(!(c1 && c2));
			carry = high + (c1 || c2);
		}
		for (size_t k = i + bn; k < rn && carry; ++k) {
			carry = add_overflow(carry, r[k], &r[k]);
		}
		overflow += carry;
	}
	return overflow;
}

void mdi_bitwise(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn,
	enum mdi_bitfun func
) {
	while (rn--) {
		digit_t ad = rn < an ? a[rn] : 0;
		digit_t bd = rn < bn ? b[rn] : 0;
		digit_t rd =
			func & mdi_bitfun_xor ? ad ^ bd :
			func & mdi_bitfun_and ? ad & bd : ad | bd;
		if (func & mdi_bitfun_inv_r) rd = ~rd;
		r[rn] = rd;
	}
}
