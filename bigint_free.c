#include <stdlib.h>

#include "bigint.h"

void bigint_free(struct bigint *b) {
	free(b);
}
