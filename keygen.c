#include "randstate.h"
#include "rsa.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#define OPTIONS "b:i:n:d:s:vh"

//prints how to use program
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "Creates a public key\n"
        "\n"
        "USAGE\n"
        " %s [b:i:n:d:s:vh] [-b minimum bits] [-i MR iterations] [-n public key file] [-d private "
        "key file] [-s seed]\n"
        "OPTIONS\n"
        " -b minimum number of bits for public modulus n\n"
        " -i number of iterations for Miller-Rabin\n"
        " -n public key file\n"
        " -d private key file\n"
        " -s seed\n"
        " -v enables verbose\n"
        " -h prints usage\n",
        exec);
}

int main(int argc, char **argv) {

    //default values
    uint64_t seed = time(NULL);
    uint64_t bits = 265;
    uint64_t iters = 50;
    char *pb = "rsa.pub";
    char *pv = "rsa.priv";
    bool verbose = false;
    int opt;

    //command line parse
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b':
            if (optarg[0] == '-') {
                printf("positive number of bits only\n");
                return EXIT_FAILURE;
            }
            bits = strtoul(optarg, NULL, 10);
            break;
        case 'i':
            if (optarg[0] == '-') {
                printf("positive number of iterations only\n");
                return EXIT_FAILURE;
            }
            iters = strtoul(optarg, NULL, 10);
            break;
        case 'n': pb = optarg; break;
        case 'd': pv = optarg; break;
        case 's': seed = strtoul(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default: return EXIT_FAILURE;
        }
    }

    FILE *pbfile = fopen(pb, "w");
    FILE *pvfile = fopen(pv, "w");

    if (pvfile == NULL) {
        printf("public file failed\n");
        return 1;
    }
    if (pbfile == NULL) {
        printf("private file failed\n");
        return 1;
    }

    //make sure private key is for user read and write only
    if (pvfile != NULL) {
        fchmod(fileno(pvfile), S_IRUSR | S_IWUSR);
    }

    randstate_init(seed);

    //initialize variables
    mpz_t p, q, n, e, d, s;
    mpz_inits(p, q, n, e, d, s, NULL);

    rsa_make_pub(p, q, n, e, bits, iters);
    rsa_make_priv(d, e, p, q);

    //get username and sign it
    char *username = getenv("USER");
    mpz_set_str(s, username, 62);
    rsa_sign(s, s, d, n);

    if (pbfile != NULL && pvfile != NULL) {
        rsa_write_pub(n, e, s, username, pbfile);
        rsa_write_priv(n, d, pvfile);
    }

    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%zu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%zu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    randstate_clear();

    mpz_clears(p, q, n, e, d, s, NULL);

    if (pvfile != NULL && pbfile != NULL) {
        fclose(pvfile);
        fclose(pbfile);
    }
    return 0;
}
