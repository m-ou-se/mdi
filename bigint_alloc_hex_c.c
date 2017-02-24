#include "bigint.h"

struct bigint *bigint_alloc_hex_c(char const *hex) {
	return bigint_alloc_hex(hex, strlen(hex));
}
