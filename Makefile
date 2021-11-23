OUT=interpret
CC=gcc
CFLAGS=-g -Wall -Wextra

all: virtual.c
	$(CC) -o $(OUT) $(CFLAGS) virtual.c

clean:
	rm $(OUT)
