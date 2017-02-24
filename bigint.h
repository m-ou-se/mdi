#include <string.h>

#include "mdi.h"

// Heap-allocated bigint.
struct bigint {
	sdigit_t ssize; // Sign and size.
	digit_t digits[];
};

struct bigint *bigint_alloc(sdigit_t ssize);

void bigint_free(struct bigint *b);

struct bigint *bigint_resize(struct bigint *b, sdigit_t ssize);

struct bigint *bigint_alloc_uint(digit_t value);
struct bigint *bigint_alloc_int(sdigit_t value);
struct bigint *bigint_alloc_copy(struct bigint *b);

// Allocate a bigint containing a + b.
struct bigint *bigint_alloc_add(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
);

// Add b to a.
// a is returned and might be reallocated, so assign the result back to a:
//   a = bigint_add(a, b->digits, b->ssize);
struct bigint *bigint_add(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
);

// Allocate a bigint containing a - b.
inline static
struct bigint *bigint_alloc_sub(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
) {
	return bigint_alloc_add(a, assize, b, -bssize);
}

// Subtract b from a.
// a is returned and might be reallocated, so assign the result back to a:
//   a = bigint_sub(a, b->digits, b->ssize);
inline static
struct bigint *bigint_sub(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
) {
	return bigint_add(a, b, -bssize);
}

// Allocate a bigint containing -b.
inline static
struct bigint *bigint_alloc_neg(digit_t const *b, sdigit_t bssize) {
	return bigint_alloc_add(NULL, 0, b, -bssize);
}

// Negate b.
inline static
void bigint_neg(struct bigint *b) {
	b->ssize = -b->ssize;
}

// Allocate a bigint containing a * b.
struct bigint *bigint_alloc_mul(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
);

// Multiply a by b.
// a is returned and might be reallocated, so assign the result back to a:
//   a = bigint_mul(a, b->digits, b->ssize);
struct bigint *bigint_mul(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
);

// Allocate a bigint containing the given hexadecimal number.
// Negative numbers should be prefixed with '-'.
struct bigint *bigint_alloc_hex(char const *hex, size_t hexlen);
struct bigint *bigint_alloc_hex_c(char const *hex);

// Always returns the number of char needed in the buffer, including null byte.
// If the return value >= buflen, nothing is written to buf.
size_t bigint_to_hex(
	digit_t const *digits, sdigit_t ssize,
	char *buf, size_t buflen
);
