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

void compress(FILE *fptIn, FILE *fptOut){
    char *allSymbols;   //list of symbols found in fptIn
    int *freqs;         //frequencies of said symbols
    //to do: do we dynamically allocate this or just pick a huge number and call it a day?
    char c;

    //build full list of symbols
    while(fread(&c, 1, 1, fptIn) == 1){
        if(containedIn(allSymbols, c, &i) == 1){
            //increment freqs
            freqs[i]++;
        } else {
            //add c to freqs
            i=0;
            while(freqs[i]!=0) i++;

            allSymbols[i] = c;
            freqs[i]=1;
        }
    }

    //build tree....
}

//returns 0 if c is not contained in list
//return 1 if is and sets i to index
int containedIn(char *list, char c, int *i){

}
