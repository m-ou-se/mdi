#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mdi.h"
#include "bigint.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

struct bigint *bigint_alloc(sdigit_t ssize) {
	struct bigint *b = (struct bigint *)malloc(sizeof(struct bigint) + sizeof(digit_t) * ABS(ssize));
	b->ssize = ssize;
	return b;
}

void bigint_free(struct bigint *b) {
	free(b);
}

struct bigint *bigint_resize(struct bigint *b, sdigit_t ssize) {
	b = (struct bigint *)realloc(b, sizeof(struct bigint) + sizeof(digit_t) * ABS(ssize));
	b->ssize = ssize;
	return b;
}

struct bigint *bigint_alloc_uint(digit_t value) {
	struct bigint *b = bigint_alloc(1);
	b->digits[0] = value;
	return b;
}

struct bigint *bigint_alloc_int(sdigit_t value) {
	struct bigint *b = bigint_alloc(value < 0 ? -1 : 1);
	b->digits[0] = value < 0 ? -(digit_t)value : value;
	return b;
}

struct bigint *bigint_alloc_copy(struct bigint *b) {
	struct bigint *c = bigint_alloc(b->ssize);
	memcpy(c->digits, b->digits, sizeof(digit_t) * ABS(b->ssize));
	return c;
}

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

struct bigint *bigint_add(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
) {
	if ((a->ssize < 0) == (bssize < 0)) {
		// Same signs, so add.
		if (mdi_add(a->digits, ABS(a->ssize), a->digits, ABS(a->ssize), b, ABS(bssize))) {
			// Carry
			a = bigint_resize(a, a->ssize + (a->ssize < 0 ? -1 : 1));
			a->digits[ABS(a->ssize) - 1] = 1;
		}
	} else {
		// Different signs, so subtract.
		sdigit_t rssize = mdi_sub(a->digits, ABS(a->ssize), a->digits, ABS(a->ssize), b, ABS(bssize));
		a->ssize = a->ssize < 0 ? -rssize : rssize;
	}
	return a;
}

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

struct bigint *bigint_mul(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
) {
	if (a->ssize == 0) return a;
	if (bssize == 0) return bigint_resize(a, 0);

	size_t const rn = ABS(a->ssize) + ABS(bssize);

	// If bssize is only 1, it might not overflow.
	if (ABS(bssize) != 1) a = bigint_resize(a, rn);

	digit_t overflow = mdi_mul(a->digits, rn, a->digits, ABS(a->ssize), b, ABS(bssize));

	if (overflow) {
		assert(ABS(bssize) == 1);
		a = bigint_resize(a, rn);
		a->digits[rn - 1] = overflow;
	}

	if (bssize < 0) a->ssize = -a->ssize;

	return a;
}

static int hex_value(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	switch (c) {
		case 'a': case 'A': return 10;
		case 'b': case 'B': return 11;
		case 'c': case 'C': return 12;
		case 'd': case 'D': return 13;
		case 'e': case 'E': return 14;
		case 'f': case 'F': return 15;
	}
	return -1;
}

struct bigint *bigint_alloc_hex(char const *hex, size_t hexlen) {
	bool sign = false;
	if (hexlen > 0 && *hex == '-') { sign = true; ++hex; --hexlen; }
	while (hexlen > 0 && *hex == '0') { ++hex; --hexlen; }
	int hex_per_digit = sizeof(digit_t) * CHAR_BIT / 4;
	sdigit_t ssize = (hexlen + hex_per_digit - 1) / hex_per_digit;
	if (sign) ssize = -ssize;
	struct bigint *b = bigint_alloc(ssize);
	size_t digit = hexlen / hex_per_digit;
	int offset = hexlen % hex_per_digit * 4;
	memset(b->digits, 0, sizeof(b->digits[0]) * ABS(ssize));
	while (hexlen--) {
		if (offset == 0) {
			offset = sizeof(digit_t) * CHAR_BIT;
			--digit;
		}
		int v = hex_value(*hex++);
		if (v < 0) {
			bigint_free(b);
			return NULL;
		}
		offset -= 4;
		b->digits[digit] |= (digit_t)v << offset;
	}
	return b;
}

struct bigint *bigint_alloc_hex_c(char const *hex) {
	return bigint_alloc_hex(hex, strlen(hex));
}

size_t bigint_to_hex(
	digit_t const *digits, sdigit_t ssize,
	char *buf, size_t buflen
) {
	char const *hex = "0123456789ABCDEF";
	size_t needed = ABS(ssize) * sizeof(digit_t) * CHAR_BIT / 4 + 1;
	if (needed < buflen) {
		for (size_t i = ABS(ssize); i--; ) {
			int shift = sizeof(digit_t) * CHAR_BIT;
			while (shift) {
				shift -= 4;
				*buf++ = hex[digits[i] >> shift & 0xF];
			}
		}
		*buf++ = '\0';
	}
	return needed;
}
