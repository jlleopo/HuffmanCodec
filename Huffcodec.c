#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void compress(FILE *fptIn, FILE *fptOut);

void decompress(FILE *fptIn, FILE *fptOut);

int main(int argc, char *argv[]){
    FILE *fptIn;
    FILE *fptOut;

    if(argc!=4) {
        printf("usage: Huffcodec 'c|d' 'fileNameIn' ' fileNameOut'");
        exit(0);
    }

    fptIn = fopen(argv[2], "rb");
    fptOut = fopen(argv[3], "wb");

    if(strcmp(argv[1], "c") == 0){
        compress(fptIn, fptOut);
    } else if(strcmp(argv[1], "d") == 0){
        decompress(fptIn, fptOut);
    } else {
        printf("second argument must be c or d");
        exit(0);
    }

	return 0;
}
