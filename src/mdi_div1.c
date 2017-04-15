#include <assert.h>

#include "mdi_impl.h"

digit_t mdi_div1(
	digit_t *q,
	digit_t const *u, size_t un,
	digit_t v
) {
	// Can't divide by zero.
	assert(v > 0);

	digit_t carry = 0;
	for (size_t i = un; --i;) {
		ddigit_t x = ((ddigit_t)carry << DIGIT_BITS) | u[i];
		if (q != NULL) q[i] = x / v;
		carry = x % v;
	}

	return carry;
}
