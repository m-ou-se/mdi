#include "../src/bigint.h"
#include "../src/mdi.h"

#include "test_impl.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

char const * comment[] = {
	"mdi_sub(r, N, a, an, b, bn)",
	"mdi_sub(a, an, a, an, b, bn)",
	"mdi_sub(b, bn, a, an, b, bn)",
	"mdi_sub(a, an, a, an, a, an)",
};

void test_sub(
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn
) {
	print_mdi(a, an); putchar('\n');
	print_mdi(b, bn); putchar('\n');
	puts("sub");

	for (int reuse = 0; reuse < 3; ++reuse) {
		size_t rn;
		if (!reuse) {
			ssize_t rssize = mdi_sub(NULL, 0, a, an, b, bn);
			rn = ABS(rssize);
			printf("%c ", rssize == 0 ? '=' : rssize < 0 ? '<' : '>');
		} else {
			rn = (reuse == 1 ? an : bn);
		}

		digit_t r[rn];
		if (reuse) memcpy(r, reuse == 1 ? a : b, (reuse == 1 ? an : bn) * sizeof(digit_t));

		ssize_t rssize = mdi_sub(r, rn, a, an, b, bn);
		if (reuse) printf("%c ", rssize == 0 ? '=' : rssize < 0 ? '<' : '>');
		print_mdi(r, rn);
		printf(" %s\n", comment[reuse]);
	}

	// a == b
	if (an == bn && memcmp(a, b, an * sizeof(digit_t)) == 0) {
		size_t rn = an;
		digit_t r[rn];
		memcpy(r, a, an * sizeof(digit_t));
		ssize_t rssize = mdi_sub(r, rn, r, rn, r, rn);
		printf("%c ", rssize == 0 ? '=' : rssize < 0 ? '<' : '>');
		print_mdi(r, rn);
		printf(" %s\n", comment[3]);
	}
}

int main() {
	struct bigint * a = read_bigint();
	struct bigint * b = read_bigint();
	test_sub(a->digits, a->ssize, b->digits, b->ssize);
	return 0;
}
