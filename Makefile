CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp) 

all: keygen encrypt decrypt 

keygen: keygen.c randstate.o numtheory.o rsa.o
	$(CC) -o keygen keygen.c randstate.o numtheory.o rsa.o $(LFLAGS)

encrypt: encrypt.c rsa.o numtheory.o randstate.o
	$(CC) -o encrypt encrypt.c rsa.o randstate.o numtheory.o $(LFLAGS)

decrypt: decrypt.c rsa.o numtheory.o randstate.o
	$(CC) -o decrypt decrypt.c rsa.o randstate.o numtheory.o $(LFLAGS)

randstate.o: 
	$(CC) $(CFLAGS) -c randstate.c 

numtheory.o:
	$(CC) $(CFLAGS) -c numtheory.c 

rsa.o: 
	$(CC) $(CFLAGS) -c rsa.c

clean:
	rm -f test keygen encrypt decrypt *.o

format:
	clang-format -i -style=file *.[c,h]
