#include "../src/bigint.h"
#include "../src/mdi.h"

#include "test_impl.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

char const * comment[] = {
	"mdi_add(r, MAX(an, bn), a, an, b, bn)",
	"mdi_add(a, an, a, an, b, bn)",
	"mdi_add(b, bn, a, an, b, bn)",
	"mdi_add(a, an, a, an, a, an)",
	"mdi_add(r, MAX(an, bn)+1, a, an, b, bn)",
	"mdi_add(a, an+1, a, an, b, bn)",
	"mdi_add(b, bn+1, a, an, b, bn)",
	"mdi_add(a, an+1, a, an, a, an)",
};

void test_add(
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn
) {
	print_mdi(a, an); putchar('\n');
	print_mdi(b, bn); putchar('\n');
	puts("add");

	for (int extra_size = 0; extra_size < 2; ++extra_size) {
		for (int reuse = 0; reuse < 3; ++reuse) {
			size_t rn = MAX(an, bn) + extra_size;
			if (reuse) rn = (reuse == 1 ? an : bn) + extra_size;

			digit_t r[rn];
			if (reuse) memcpy(r, reuse == 1 ? a : b, (reuse == 1 ? an : bn) * sizeof(digit_t));

			bool overflow = mdi_add(r, rn, a, an, b, bn);
			printf("%d:", overflow);
			print_mdi(r, rn);
			printf(" %s\n", comment[extra_size * 4 + reuse]);
		}

		// a == b
		if (an == bn && memcmp(a, b, an * sizeof(digit_t)) == 0) {
			size_t rn = an + extra_size;
			digit_t r[rn];
			memcpy(r, a, an * sizeof(digit_t));
			bool overflow = mdi_add(r, rn, r, rn, r, rn);
			printf("%d", overflow);
			print_mdi(r, rn);
			printf(" %s\n", comment[extra_size * 4 + 3]);
		}
	}
}

int main() {
	struct bigint * a = read_bigint();
	struct bigint * b = read_bigint();
	test_add(a->digits, a->ssize, b->digits, b->ssize);
	return 0;
}
