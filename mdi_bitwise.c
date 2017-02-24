#include "mdi.h"

void mdi_bitwise(
	digit_t * r, size_t rn,
	digit_t const * a, size_t an,
	digit_t const * b, size_t bn,
	enum mdi_bitfun func
) {
	while (rn--) {
		digit_t ad = rn < an ? a[rn] : 0;
		digit_t bd = rn < bn ? b[rn] : 0;
		digit_t rd =
			func & mdi_bitfun_xor ? ad ^ bd :
			func & mdi_bitfun_and ? ad & bd : ad | bd;
		if (func & mdi_bitfun_inv_r) rd = ~rd;
		r[rn] = rd;
	}
}
