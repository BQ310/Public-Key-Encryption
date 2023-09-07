#include "randstate.h"
#include <stdlib.h>

// initialize global variable
gmp_randstate_t state;

// initializes random with seed
void randstate_init(uint64_t seed) {
    srandom(seed);
    // gmp uses mersenne twister
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

//clears the random state
void randstate_clear(void) {
    gmp_randclear(state);
}
