CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS =

all: libmlpt.a

libmlpt.a: mlpt.o
	ar rcs $@ $^

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c mlpt.c

clean:
	rm -f *.o *.a

.PHONY: all clean
