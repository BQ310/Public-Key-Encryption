#include <stdbool.h>
#include <stdio.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"
#include <sys/stat.h>
#include <stdlib.h>

int main(void) {
	
	mpz_t l, e, ngcd;
	mpz_inits(l, e, ngcd, NULL);

	mpz_set_ui(l, 3505178077862068994308457252477019643710117487045830178701129352555864997018700);
	mpz_set_ui(e, 34400321579674930317064500546366555594368205306063833940664858093892962913592129);

	gcd(ngcd, l, e);

	gmp_printf("gcd:%Zd\n", ngcd);
	
	mpz_clears(l, e, ngcd, NULL);

}
