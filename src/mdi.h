#pragma once

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef DIGIT_BITS
#if UINTPTR_MAX == UINT64_MAX
#define DIGIT_BITS 64
#else
#define DIGIT_BITS 32
#endif
#endif

#if DIGIT_BITS == 64
#define DIGIT_MAX 0xFFFFFFFFFFFFFFFF
typedef uint64_t digit_t;
typedef int64_t sdigit_t;

#elif DIGIT_BITS == 32
#define DIGIT_MAX 0xFFFFFFFF
typedef uint32_t digit_t;
typedef int32_t sdigit_t;

#else
#error DIGIT_BITS is neither 64 nor 32.
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Add two (unsigned) integers: r = a + b
// Returns the carry bit.
// Requires rn >= max(an, bn). When rn > max(an, bn), a carry cannot occur.
// r, a, and b may be equal. r may not otherwise overlap with a or b.
// To correct for a carry, simply add a (most significant) digit containing 1
// to the result.
bool mdi_add(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
);

// Subtract two (unsigned) integers: r = |a - b|
// Requires rn >= N.
// N is the number of digits in the result without leading zeros.
// N is always <= max(an, bn).
// Returns:
//    N if a > b, so r = a - b
//    0 if a = b, so r = 0 (and N = 0)
//   -N if b < a, so r = b - a
// r, a, and b may be equal. r may not otherwise overlap with a or b.
// r may be NULL if you're only want to determine N, or just compare a and b.
// If rn is lower than an or bn, only the first rn digits of a and b are
// considered, except when r = NULL and rn = 0. Also, when r = NULL, only the
// last N digits are checked. This means that calling the function first with
// r = NULL and rn = 0 to determine N, and then with r != NULL and rn = N (if N
// != 0) is efficient.
// Overflow cannot occur, since 0 <= |a - b| <= max(a, b).
sdigit_t mdi_sub(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
);

// Compare two (unsigned) integers.
// Returns:
//    N if a > b
//    0 if a = b
//   -N if a < b
// N is the lowest N for which a[N - 1] != b[N - 1].
// This means ABS(return value) is the number of digits of |a - b|.
// (See mdi_sub, this is just mdi_sub with r = NULL and rn = 0.)
inline static
sdigit_t mdi_cmp(
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
) {
	return mdi_sub(NULL, 0, a, an, b, bn);
}

// Adds or subtracts signed 2's complement integers.
// Stores the result of a + b or a - b in r, and returns true on overflow.
// Set negate_b to true to calculate a - b, or false for a + b.
// Requires rn >= max(an, bn).
// When rn > max(an, bn), overflow cannot occur.
// r, a, and b may be equal. r may not otherwise overlap with a or b.
// Overflow simply means the result is correct, except for the sign. Correct it
// by adding a (most significant) digit with all bits set opposite to the sign
// of the result.
bool mdi_add_signed(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn,
	bool negate_b
);

// Multiplies (unsigned) integers (r = a * b).
// Requires rn >= an + bn - 1.
// When rn >= an + bn, overflow cannot occur.
// Returns the digit that should be appended to the result (the new
// most-significant digit), which is zero unless there is an overflow.
// a and r may be equal. b may not be equal to r. r may not otherwise overlap
// with a or b.
digit_t mdi_mul(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
);

// Divide a by (single digit) b.
// Quotient is stored in q, and has size an.
// The buffer of q may overlap with a, as long as &q[0] >= &a[0].
// The remainder is returned.
// b may not be 0.
// q may be NULL, if you only want to calculate the remainder.
digit_t mdi_div1(
	digit_t *q,
	digit_t const *a, size_t an,
	digit_t b
);

// Divide a by b.
// The quotient will be written to q, and has size an - bn + 1 if an >= bn, or
// 0 otherwise. (If an < bn, there's no need to call this function. It is a
// no-op in that case.)
// The remainder will be in the first min(an, bn) digits of a.
// The buffer of q may overlap with u, as long as &q[0] > &a[bn].
// The buffer of b may not overlap with q or a.
// b may not have any leading zeros, and bn must be larger than zero. (So, b
// cannot be zero.)
// q may be NULL, if you only want to calculate the remainder.
void mdi_div(
	digit_t *q,
	digit_t *a, size_t an,
	digit_t const *b, size_t bn
);

// r = a ^ b
// TODO
bool mdi_pow(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn
);

// r = a ^ b mod c
// TODO
bool mdi_powmod(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn,
	digit_t const * c, size_t cn
);

// r = sqrt(a)
// TODO
bool mdi_sqrt(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an
);

enum mdi_bitfun {
	// Basic operations:
	mdi_bitfun_or = 0,
	mdi_bitfun_and = 7,
	mdi_bitfun_xor = 8,

	// Modifiers, which must be xorred to apply them:
	mdi_bitfun_inv_r = 1,
	mdi_bitfun_inv_a = 2,
	mdi_bitfun_inv_b = 4,

	// All combinations of the above operations with the modifiers:
	mdi_bitfun_a_or_b     = 0,
	mdi_bitfun_a_or_nb    = 4,
	mdi_bitfun_na_or_b    = 2,
	mdi_bitfun_na_or_nb   = 6,

	mdi_bitfun_a_nor_b    = 1,
	mdi_bitfun_a_nor_nb   = 5,
	mdi_bitfun_na_nor_b   = 3,
	mdi_bitfun_na_nor_nb  = 7,

	mdi_bitfun_a_and_b    = 7,
	mdi_bitfun_a_and_nb   = 3,
	mdi_bitfun_na_and_b   = 5,
	mdi_bitfun_na_and_nb  = 1,

	mdi_bitfun_a_nand_b   = 6,
	mdi_bitfun_a_nand_nb  = 2,
	mdi_bitfun_na_nand_b  = 4,
	mdi_bitfun_na_nand_nb = 0,

	mdi_bitfun_a_xor_b    = 8,
	mdi_bitfun_na_xor_b   = 9,
	mdi_bitfun_a_xor_nb   = 9,
	mdi_bitfun_na_xor_nb  = 8,

	mdi_bitfun_a_nxor_b   = 9,
	mdi_bitfun_na_nxor_b  = 8,
	mdi_bitfun_a_nxor_nb  = 8,
	mdi_bitfun_na_nxor_nb = 9,
};

// Applies the bitwise function specified by func to a and b, and stores the
// result in r.
// rn may be smaller than an and/or bn, only up to rn digits are considered.
void mdi_bitwise(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn,
	enum mdi_bitfun func
);

// r = a << shift
// TODO
bool mdi_shift(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	size_t shift
);

#ifdef __cplusplus
}
#endif
