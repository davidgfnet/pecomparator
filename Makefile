
CC=gcc
CFLAGS=-O2

all:
	$(CC) $(CFLAGS) -o pecomparator pecomparator.c

clean:
	rm -f pecomparator

