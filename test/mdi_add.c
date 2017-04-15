#include "../src/mdi.h"
#include "../src/bigint.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))

void print_mdi(digit_t const * digits, sdigit_t size) {
	size_t buffer_size = bigint_to_hex(digits, size, NULL, 0);
	char * buffer = malloc(buffer_size);
	bigint_to_hex(digits, size, buffer, buffer_size);
	fputs(buffer, stdout);
	free(buffer);
}

char const * reuse_comments[] = {
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
	digit_t const * a, sdigit_t an,
	digit_t const * b, sdigit_t bn
) {
	print_mdi(a, an); putchar('\n');
	print_mdi(b, bn); putchar('\n');
	puts("add");

	for (int extra_size = 0; extra_size < 2; ++extra_size) {
		for (int reuse = 0; reuse < 3; ++reuse) {
			size_t rn = MAX(an, bn) + extra_size;
			if (reuse) rn = (reuse == 1 ? an : bn) + extra_size;

			digit_t r[rn];
			if (reuse) memcpy(r, reuse == 1 ? a : b, reuse == 1 ? an : bn);

			bool overflow = mdi_add(r, rn, a, an, b, bn);
			printf("%d", overflow);
			print_mdi(r, rn);
			printf(" %s\n", reuse_comments[extra_size * 4 + reuse]);
		}

		// a == b
		if (an == bn && memcmp(a, b, an) == 0) {
			size_t rn = an + extra_size;
			digit_t r[rn];
			memcpy(r, a, an);
			bool overflow = mdi_add(r, rn, r, rn, r, rn);
			printf("%d", overflow);
			print_mdi(r, rn);
			printf(" %s\n", reuse_comments[extra_size * 4 + 3]);
		}
	}
}

struct bigint * read_bigint() {
	char * line = NULL;
	size_t buffer_size = 0;
	ssize_t line_size;
	line_size = getline(&line, &buffer_size, stdin);
	if (line_size < 0) {
		perror("getline");
		exit(1);
	}

	while (line_size && isspace(line[line_size - 1])) --line_size;
	if (line_size == 0) {
		fprintf(stderr, "expected hex bigint, got empty line");
		exit(1);
	}

	return bigint_alloc_hex(line, line_size);
}

int main() {
	struct bigint * a = read_bigint();
	struct bigint * b = read_bigint();
	test_add(a->digits, a->ssize, b->digits, b->ssize);
	return 0;
}
