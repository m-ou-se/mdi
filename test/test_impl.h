#include "../src/bigint.h"
#include "../src/mdi.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static inline void print_mdi(digit_t const * digits, size_t size) {
	if (size == 0) {
		printf("0");
		return;
	}
	for (size_t i = size; i--;) {
		printf("%0*lX", (int)sizeof(digit_t) * 2, (unsigned long)digits[i]);
		if (i) putchar(':');
	}
}

static inline struct bigint * read_bigint() {
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
