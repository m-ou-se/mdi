#include <stdlib.h>

#include "bigint_impl.h"

struct bigint *bigint_alloc(sdigit_t ssize) {
	struct bigint *b = (struct bigint *)malloc(sizeof(struct bigint) + sizeof(digit_t) * ABS(ssize));
	b->ssize = ssize;
	return b;
}
