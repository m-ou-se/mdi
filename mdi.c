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
	assert(rn >= an);
	assert(rn >= bn);
	int carry = 0;
	for (size_t i = 0; i < rn; ++i) {
		digit_t ad = i < an ? a[i] : 0;
		digit_t bd = i < bn ? b[i] : 0;
		digit_t rd;
		carry = add_overflow(ad, carry, &rd);
		carry |= add_overflow(rd, bd, &rd);
		r[i] = rd;
	}
	return rn && carry;
}

sdigit_t mdi_sub_abs(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	// The number of digits we'll consider.
	size_t n = rn;
	if (r == NULL && n == 0) n = an > bn ? an : bn;

	// Find the common prefix and check which one is larger.
	bool a_lt_b = false;
	while (n--) {
		digit_t ad = n < an ? a[n] : 0;
		digit_t bd = n < bn ? b[n] : 0;
		if (ad < bd) { a_lt_b = true; break; }
		if (ad > bd) { a_lt_b = false; break; }
	}
	++n;

	if (r != NULL && n > 0) {

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
