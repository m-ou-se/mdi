#include <stddef.h>
#include <string.h>

#include "mdi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Heap-allocated bigint.
struct bigint {
	sdigit_t ssize; // Sign and size.
	digit_t digits[];
};

void bigint_free(struct bigint *b);

struct bigint *bigint_resize(struct bigint *b, sdigit_t ssize);

// Allocate a bigint, with uninitialized digits.
// Overwrite the digits before using. (Except when ssize == 0.)
struct bigint *bigint_alloc(sdigit_t ssize);

struct bigint *bigint_alloc_copy(digit_t const *digits, sdigit_t ssize);

inline static
struct bigint *bigint_alloc_zero() {
	return bigint_alloc(0);
}

inline static
struct bigint *bigint_alloc_uint(digit_t value) {
	return bigint_alloc_copy(&value, value == 0 ? 0 : 1);
}

inline static
struct bigint *bigint_alloc_int(sdigit_t value) {
	digit_t absval = value < 0 ? -(digit_t)value : value;
	return bigint_alloc_copy(&absval, value == 0 ? 0 : value < 0 ? -1 : 1);
}

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
	return bigint_alloc_copy(b, -bssize);
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

// Allocate a bigint containing a / b.
// Requires b != 0.
struct bigint *bigint_alloc_div(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
);

// Allocate a bigint containing a % b.
// Requires b != 0.
struct bigint *bigint_alloc_rem(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
);

// Store the remainder of a divided by b in a.
// Requires b != 0.
void bigint_rem(
	struct bigint *a,
	digit_t const *b, sdigit_t bssize
);

// Return a^b.
// b is just a single digit, since larger numbers would result in an overflow
// anyway (except when a is 0 or 1).
// Requires that a and b are not both zero.
// Note that b is unsigned. (Negative powers wouldn't result in an integer.)
struct bigint *bigint_alloc_pow(
	digit_t const *a, sdigit_t assize,
	digit_t b
);

// Return a^b % m.
// TODO
struct bigint *bigint_alloc_powmod(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize,
	digit_t const *m, sdigit_t mssize
);

// Compare a and b.
// Returns:
//   <0 if a < b
//    0 if a = b
//   >0 if a > b
int bigint_cmp(
	digit_t const *a, sdigit_t assize,
	digit_t const *b, sdigit_t bssize
);

// Allocate a bigint containing the given hexadecimal number.
// Negative numbers should be prefixed with '-'.
// Returns NULL if the given string contains invalid characters.
struct bigint *bigint_alloc_hex(char const *hex, size_t hexlen);
struct bigint *bigint_alloc_hex_c(char const *hex);

// Always returns the number of char needed in the buffer, including null byte.
// If the return value >= buflen, nothing is written to buf.
size_t bigint_to_hex(
	digit_t const *digits, sdigit_t ssize,
	char *buf, size_t buflen
);

#ifdef __cplusplus
}
#endif
