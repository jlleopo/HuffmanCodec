
CC=clang
CFLAGS=-Wall -g

Huff: Huffcodec.c
	$(CC) $(CFLAGS) Huffcodec.c -o HuffCodec
