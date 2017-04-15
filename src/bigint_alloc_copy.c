#include <stdlib.h>

#include "bigint_impl.h"

struct bigint *bigint_alloc_copy(digit_t const *digits, sdigit_t ssize) {
	struct bigint *b = bigint_alloc(ssize);
	memcpy(b->digits, digits, sizeof(digit_t) * ABS(ssize));
	return b;
}
