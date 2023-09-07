#include "numtheory.h"
#include "randstate.h"

//calculates the greatest common demoninator of two numbers
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t temp;
    mpz_init(temp);

    //uses temp due to pass by reference
    while (mpz_cmp_ui(b, 0) != 0) {
        mpz_set(temp, b);
        mpz_mod(b, a, temp);
        mpz_set(a, temp);
    }
    mpz_set(d, a);
    mpz_clear(temp);
}

// calculates the modular inverse where i * a = 1 (mod n)
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {

    mpz_t r, rs, t, ts, q, temp;
    mpz_inits(r, rs, t, ts, q, temp, NULL);
    mpz_set(r, n);
    mpz_set(rs, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(ts, 1);

    //while rs != 0
    while (mpz_cmp_ui(rs, 0) != 0) {
        mpz_tdiv_q(q, r, rs); //q = r/rs

        mpz_set(temp, rs); //save rs
        mpz_mul(rs, rs, q); //q * rs
        mpz_sub(rs, r, rs); //rs = r - q * rs
        mpz_set(r, temp); //r = old rs

        mpz_set(temp, ts); //save ts
        mpz_mul(ts, q, ts); //q * ts
        mpz_sub(ts, t, ts); //ts = t - q * ts
        mpz_set(t, temp); //t = old ts
    }

    //if t < 0
    if (mpz_cmp_ui(t, 0) < 0) {
        //t = t + n
        mpz_add(t, t, n);
    }
    mpz_set(i, t);
    // if r > 1
    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(i, 0);
    }

    mpz_clears(q, temp, t, ts, r, rs, NULL);
}

//calculates (base^exponent)(mod modulus) and places result in out
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t p;
    mpz_t v;
    mpz_inits(p, v, NULL);
    mpz_set(p, base);
    mpz_set_ui(v, 1);

    while (mpz_cmp_ui(exponent, 0) > 0) { // while (exponent > 0)
        if (mpz_odd_p(exponent)) {
            mpz_mul(v, v, p); // v = (v * p) mod modulus
            mpz_mod(v, v, modulus);
        }
        mpz_mul(p, p, p); // p = p^2 mod modulus
        mpz_mod(p, p, modulus);
        mpz_tdiv_q_ui(exponent, exponent, 2); // exponent = exponent / 2
    }
    mpz_set(out, v);
    mpz_clears(p, v, NULL);
}

//assumes random state is already initialized
//checks if a number is prime
bool is_prime(mpz_t n, uint64_t iters) {

    // 1 is not a prime number
    // 3 causes errors when calculating the random number
    if (mpz_cmp_ui(n, 1) == 0) {
        return false;
    }

    if (mpz_cmp_ui(n, 3) == 0) {
        return true;
    }

    mpz_t r, s, a, y, j, subn, two;
    mpz_inits(r, s, a, y, j, subn, two, NULL);

    mpz_set_ui(two, 2);

    mpz_sub_ui(r, n, 1); //r = n - 1
    mpz_set_ui(s, 0); //the power of 2

    // while r is not odd divide r by 2 and add 1 to s
    while (!mpz_odd_p(r)) {
        mpz_tdiv_q_ui(r, r, 2);
        mpz_add_ui(s, s, 1);
    }
    // anything less than or equal to 3 will break urandomm which is taken care of
    // using if statements
    mpz_sub_ui(subn, n, 3);

    for (uint64_t i = 1; i <= iters; i++) {
        //a 0...n - 4
        mpz_urandomm(a, state, subn);
        //a 2 .. n - 2
        mpz_add_ui(a, a, 2);
        //subn = n - 1
        mpz_sub_ui(subn, n, 1);
        pow_mod(y, a, r, n);
        //y != 1 && y != n - 1
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, subn))) {
            mpz_set_ui(j, 1);
            mpz_sub_ui(s, s, 1);
            // while j > s and y != n - 1
            while ((mpz_cmp(j, s) != -1) && (mpz_cmp(y, subn) != 0)) {
                // y = y^2 mod n
                pow_mod(y, y, two, n);
                //if y == 1
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(r, s, a, y, j, subn, two, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            //if y != n - 1
            if (mpz_cmp(y, subn) != 0) {
                mpz_clears(r, s, a, y, j, subn, two, NULL);
                return false;
            }
        }
    }

    mpz_clears(r, s, a, y, j, subn, two, NULL);

    return true;
}
//assumes random state is initialized
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    // makes random mpz_t of arount with bits amount of bits
    mpz_rrandomb(p, state, bits);

    //redo until p is prime
    while (!is_prime(p, iters)) {
        mpz_urandomb(p, state, bits);
    }
}
