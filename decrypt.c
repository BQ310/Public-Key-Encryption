#include "rsa.h"
#include <unistd.h>
#include <stdlib.h>

#define OPTIONS "i:o:n:vh"

// prints usage of program

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "Decrypts an encrypted message.\n"
        "\n"
        "USAGE\n"
        " %s [i:o:n:vh] [-i input file] [-o output file] [-n private key]\n"
        " -i input file\n"
        " -o output file\n"
        " -n private key file\n"
        " -v verbose\n"
        " -h usage\n",
        exec);
}

//  decrypts an message

int main(int argc, char **argv) {
    FILE *infile = stdin;
    char *pv = "rsa.priv";
    FILE *outfile = stdout;
    bool verbose = false;
    int opt;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                printf("input file did not register\n");
                return EXIT_FAILURE;
            }
            break;

        case 'o':
            outfile = fopen(optarg, "w");
            if (outfile == NULL) {
                printf("output file did not register\n");
                return EXIT_FAILURE;
            }
            break;
        case 'n': pv = optarg; break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default: return EXIT_FAILURE;
        }
    }

    FILE *pvfile = fopen(pv, "r");
    if (pvfile == NULL) {
        printf("pvfile did not register\n");
    }

    mpz_t d, n;
    mpz_inits(d, n, NULL);

    if (pvfile != NULL) {
        rsa_read_priv(n, d, pvfile);
    }

    //prints n and d along with their bits
    if (verbose) {
        gmp_printf("n (%zu bits): %Zd\nd (%zu bits) %Zd\n", mpz_sizeinbase(n, 2), n,
            mpz_sizeinbase(d, 2), d);
    }

    if (infile != NULL && outfile != NULL) {
        rsa_decrypt_file(infile, outfile, n, d);
    }

    mpz_clears(n, d, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pvfile);
    return 0;
}
