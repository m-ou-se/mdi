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
typedef __uint128_t ddigit_t;

#elif DIGIT_BITS == 32
#define DIGIT_MAX 0xFFFFFFFF
typedef uint32_t digit_t;
typedef int32_t sdigit_t;
typedef uint64_t ddigit_t;

#else
#error DIGIT_BITS is neither 64 nor 32.
#endif

// Add two (unsigned) integers: r = a + b
// Returns the carry bit.
// Requires rn >= max(an, bn). When rn > max(an, bn), a carry cannot occur.
// r, a, and b may be equal.
// To correct for a carry, simply add a (most significant) digit containing 1
// to the result.
bool mdi_add(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
);

// Subtract two (unsigned) integers: r = |a - b|
// Only with rn >= N, a correct result is produced.
//   N is the number of digits in the result without leading zeros.
//   N is always <= max(an, bn).
// Returns:
//    N if a > b, so r = a - b
//    0 if a = b, so r = 0
//   -N if b < a, so r = b - a
// r, a, and b may be equal.
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
// This means N is the number of digits of |a - b|.
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
// r, a, and b may be equal.
// Overflow simply means the result is correct, except for the sign. Correct it
// by adding a (most significant) digit with all bits set opposite to the sign
// of the result.
bool mdi_add_signed(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn,
	bool negate_b
);

// Multiplies (unsigned) integers (r = a * b).. Returns true on overflow.
// When rn >= an + bn, overflow cannot occur.
// a and r may be equal. b may not be equal to r.
// On overflow,  you can't easily correct the result as with addition or
// subtraction, since information is lost.
bool mdi_multiply(
	digit_t *r, size_t rn,
	digit_t const *a, size_t an,
	digit_t const *b, size_t bn
);

// Divides a by b, stores the result in div, and the remainder in rem.
// One of div and rem may be NULL.
bool mdi_divide(
	digit_t * div, size_t divn,
	digit_t * rem, size_t remn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn
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
	mdi_bitfun_and = 4,
	mdi_bitfun_xor = 8,

	// Modifiers, which must be xorred to apply them:
	mdi_bitfun_inv_r = 1,
	mdi_bitfun_inv_a = 2,
	mdi_bitfun_inv_b = 7,

	// All combinations of the above operations with the modifiers:
	mdi_bitfun_a_or_b     = 0,
	mdi_bitfun_a_or_nb    = 7,
	mdi_bitfun_na_or_b    = 2,
	mdi_bitfun_na_or_nb   = 5,

	mdi_bitfun_a_nor_b    = 1,
	mdi_bitfun_a_nor_nb   = 6,
	mdi_bitfun_na_nor_b   = 3,
	mdi_bitfun_na_nor_nb  = 4,

	mdi_bitfun_a_and_b    = 4,
	mdi_bitfun_a_and_nb   = 3,
	mdi_bitfun_na_and_b   = 6,
	mdi_bitfun_na_and_nb  = 1,

	mdi_bitfun_a_nand_b   = 5,
	mdi_bitfun_a_nand_nb  = 2,
	mdi_bitfun_na_nand_b  = 7,
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
