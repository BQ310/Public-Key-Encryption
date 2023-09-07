#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>
#include <inttypes.h>

//calculates the lambda of p - 1 and n - 1
void lambda(mpz_t nlambda, mpz_t p, mpz_t q) {
    mpz_t pm, qm, grcd, totient;
    mpz_inits(pm, qm, grcd, totient, NULL);
    mpz_sub_ui(pm, p, 1); // pm = p - 1
    mpz_sub_ui(qm, q, 1); // qm = q - 1
    mpz_mul(totient, qm, pm); // totient = (p - 1)(q - 1)
    gcd(grcd, qm, pm); // grcd = gcd(p - 1, q - 1)
    mpz_tdiv_q(nlambda, totient, grcd); // lambda = totient / grcd;
    mpz_clears(pm, qm, grcd, totient, NULL);
}

// creates a public key
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {

    //random number for bits of p
    uint64_t bitsp = random();
    mpz_t nlambda, grcd, exp, temp;
    mpz_inits(nlambda, grcd, exp, temp, NULL);

    bitsp = bitsp % ((3 * nbits / 4) + 1); // bitsp will be less than 3/4 nbits inclusive
    if (bitsp < (nbits / 4)) { // bitsp must be greater than 1/4 nbits
        bitsp = bitsp + (nbits / 4);
    }
    uint64_t bitsq = nbits - bitsp;

    //p and q are prime numbers with their respective #bits
    make_prime(p, bitsp, iters);
    make_prime(q, bitsq, iters);
    mpz_mul(n, p, q); // n = p x q

    //nlambda = lambda(p - 1, q - 1)
    lambda(nlambda, p, q);
    mpz_set(temp, nlambda);

    //get random e until gcd(e, lambda) == 1
    do {
        mpz_urandomb(exp, state, nbits);
        mpz_set(e, exp);
        gcd(grcd, temp, exp);
        mpz_set(temp, nlambda);
    } while (mpz_cmp_ui(grcd, 1) != 0); //while e and lambda are not coprime
    mpz_clears(nlambda, grcd, exp, temp, NULL);
}

//writes the public key values as hexstrings with trailing newlines
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    gmp_fprintf(pbfile, "%s", username);
}

// reads a public key file
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s", n, e, s, username);
}

//creates a private key
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t nlambda;
    mpz_init(nlambda);
    //nlambda = lambda (p - 1, q - 1)
    lambda(nlambda, p, q);
    // d * e = 1 (mod lambda(p - 1, q - 1))
    mod_inverse(d, e, nlambda);
    mpz_clear(nlambda);
}

//writes a private key to a file as hexstrings with a trailing newline
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

//reads values from a private key file
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

// encrypts a message m^e mod n
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    mpz_t exp;
    mpz_init(exp);
    mpz_set(exp, e);
    // m^e mod n
    pow_mod(c, m, exp, n);
    mpz_clear(exp);
}

// encrypts infile in blocks then outputs to outfile
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t m, c;
    mpz_inits(m, c, NULL);

    //k = size of block
    //k = (log2(n) - 1) / 8
    size_t k = mpz_sizeinbase(n, 2);
    k = (k - 1) / 8;

    // create a array k in length padded with 0xFF
    uint8_t *bytes = (uint8_t *) malloc(k * sizeof(uint8_t));

    bytes[0] = 0xFF;

    size_t j = 0;

    //fread from eugene lab section
    //read in k - 1 bytes from infile
    while ((j = fread((bytes + 1), sizeof(uint8_t), (k - 1), infile)) > 0) {
        //turns the read bytes into a mpz_t
        mpz_import(m, (j + 1), 1, sizeof(uint8_t), 1, 0, bytes);
        //encrypt the block and then print to outfile
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    free(bytes);
    bytes = NULL;
    mpz_clears(m, c, NULL);
}

// decrypts a message c^d mod n
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    mpz_t exp;
    mpz_init(exp);
    mpz_set(exp, d);
    //c^d mod n
    pow_mod(m, c, exp, n);
    mpz_clear(exp);
}

//decrypts a file in blocks
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t c, m;
    mpz_inits(m, c, NULL);

    // k = size of block
    size_t k = mpz_sizeinbase(n, 2);
    k = (k - 1) / 8;

    uint8_t *bytes = (uint8_t *) malloc(k * sizeof(uint8_t));

    // # of bytes to be written from a block
    size_t count = 0;
    size_t *j = &count;

    // read in a hexstring from the encoded file
    while (gmp_fscanf(infile, "%Zx\n", c) > 0) {
        // decrypt the block
        rsa_decrypt(m, c, d, n);
        // turn the decoded mpz block into an array of bytes
        mpz_export(bytes, j, 1, sizeof(uint8_t), 1, 0, m);
        // write from +1 to ignore padding
        fwrite((bytes + 1), sizeof(uint8_t), count - 1, outfile);
    }

    mpz_clears(c, m, NULL);
    free(bytes);
    bytes = NULL;
}

// signs value using private key
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    mpz_t exp;
    mpz_init(exp);
    mpz_set(exp, d);

    //s = m^d mod n
    pow_mod(s, m, exp, n);
    mpz_clear(exp);
}

// verifies if a decoded signature is the same as the message which made it
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t exp;
    mpz_init(exp);
    mpz_set(exp, e);
    // s = m^d mod n
    // s = m^de mod n = m
    pow_mod(s, s, exp, n);

    mpz_clear(exp);

    //if the dedcoded sig == message then the sig is verified
    if (mpz_cmp(m, s) == 0) {
        return true;
    }
    return false;
}
