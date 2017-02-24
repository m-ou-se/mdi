#include "bigint_impl.h"

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
