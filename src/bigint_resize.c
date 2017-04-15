#include <stdlib.h>

#include "bigint_impl.h"

struct bigint *bigint_resize(struct bigint *b, sdigit_t ssize) {
	b = (struct bigint *)realloc(b, sizeof(struct bigint) + sizeof(digit_t) * ABS(ssize));
	b->ssize = ssize;
	return b;
}
