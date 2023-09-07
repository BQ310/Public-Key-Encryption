#include "rsa.h"
#include <unistd.h>
#include <stdlib.h>

#define OPTIONS "i:o:n:vh"

//prints use of program with command line arguments
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "Encrypts message\n"
        "\n"
        "USAGE\n"
        " %s [i:o:n:vh] [-i input file] [-o output file] [-n public key file]\n"
        "OPTIONS\n"
        " -i gets input file\n"
        " -o gets output file\n"
        " -n gets public key file\n"
        " -v verbose\n"
        " -h prints usage\n",
        exec);
}

int main(int argc, char **argv) {
    //default values
    FILE *infile = stdin;
    FILE *outfile = stdout;
    char *key = "rsa.pub";
    bool verbose = false;
    int opt;

    //getopt loop to get command line arguments
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
        case 'n': key = optarg; break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default: return EXIT_FAILURE;
        }
    }

    FILE *keyfile = fopen(key, "r");

    if (keyfile == NULL) {
        printf("need key\n");
        return EXIT_FAILURE;
    }

    mpz_t n, e, s, user;
    mpz_inits(n, e, s, user, NULL);

    //from awano on class discord
    //originally had uninitialized pointer
    //username larger than 255 char will not fit
    char username[256] = { 0 };

    rsa_read_pub(n, e, s, username, keyfile);

    //prints n, e, s and their bits if -v
    if (verbose) {
        gmp_printf("user:%s\ns (%zu bits): %Zd\nn (%zu bits): %Zd\ne: (%zu bits): %Zd\n", username,
            mpz_sizeinbase(s, 2), s, mpz_sizeinbase(n, 2), n, mpz_sizeinbase(e, 2), e);
    }

    mpz_set_str(user, username, 62);

    //verifies username with signature
    if (!rsa_verify(user, s, e, n)) {
        printf("wrong user\n");
        return EXIT_FAILURE;
    }

    rsa_encrypt_file(infile, outfile, n, e);

    mpz_clears(n, e, s, user, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(keyfile);
}
