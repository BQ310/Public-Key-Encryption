# Assignment 5: Public Key Cryptography

## Description:
Creates a public and private key pair that can be used to encrypt and decrypt messages.

## Build:
1. make/make all - creates encrypt, decrypt, and keygen executables
2. make encrypt/make decrypt/make keygen - makes its specified executable
3. make clean - removes all *.o files and all executables created by make
4. make format - formats all *.c and *.h files

## Command line arguments:
keygen:
    -b minimum number of bits for the public key
    -i number of iterations for Miller-Rabin
    -n the name of the public key file
    -d the name of the private key file
    -s the seed
    -v verbose (it prints out the value of important variables)
    -h prints how to use program
encrypt:
    -i the name of the input file to encrypt
    -o the name of the output file to put encrypted hexstrings
    -n the public key file
    -v prints important variables
    -h prints how to use program
decrypt:
    -i the encrpyted file to be decrypted
    -o the output file of the decrypted result
    -n the private key
    -v prints value of important variables
    -h prints how to use the program

## Running
./keygen -b # -i # -n public_file_name -d private_file_name -s # -v
Any iteration of the following will create a public and private key and store them within the given files.

./keygen will use defaults and is equivalent to ./keygen -b 256 -i 50 -n rsa.pub -d rsa.priv -s time

./encrypt -i infile -o outfile -n public_key -v
Any iteration of the above will encrypt the message in infile using public_key and output to outfile

./encrypt -i infile -o outfile defaults to ./encrypt -i infile -o outfile -n rsa.pub

./decrypt -i encrypted -o decrypted -n private_key -v
Any iteration of the above will decrypt the message in encrypted using the private_key and output to decrypted

if -n is not listed it will default to rsa.priv

## Scan build
No scan build errors

## Valgrind
No memory leaks



