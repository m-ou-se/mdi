#include "bigint_impl.h"

size_t bigint_to_hex(
	digit_t const *digits, sdigit_t ssize,
	char *buf, size_t buflen
) {
	if (ssize == 0) {
		if (buflen >= 2) {
			*buf++ = '0';
			*buf++ = '\0';
		}
		return 2;
	}
	char const *hex = "0123456789ABCDEF";
	size_t needed = ABS(ssize) * sizeof(digit_t) * CHAR_BIT / 4 + 1 + (ssize < 0);
	if (needed < buflen) {
		if (ssize < 0) *buf++ = '-';
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
