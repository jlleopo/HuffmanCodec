#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIGNUM 1000

typedef struct node{
  char symb;
  struct node *left;
  struct node *right;
} t_node;

void compress(FILE *fptIn, FILE *fptOut);

void decompress(FILE *fptIn, FILE *fptOut);

int main(int argc, unsigned char *argv[]){
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
    unsigned char *allSymbols;   //list of symbols found in fptIn
    int *freqs;         //frequencies of said symbols
    unsigned char c;

    t_node *ptrees;
    t_node ftree;

    allSymbols = (unsigned char *) calloc(BIGNUM, sizeof(unsigned char));
    freqs = (int *) calloc(BIGNUM, sizeof(int));


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
    //sort
    mSort(char *cs, int *freqs);
}

//returns 0 if c is not contained in list
//return 1 if is and sets i to index
int containedIn(unsigned char *list, unsigned char c, int *i){
    int j;
    for(j=0; j<BIGNUM; j++){
        if(list[j] == c){
            *i = j;
            return 1;
        }
    }
    return 0;
}

void mSort(unsigned char *cs, int *freqs){
    int i, j, k;
    int len=0;
    int swapped;
    char ctemp;
    int itemp;

    while(freqs[len]!=0) len++;

    do{
        swapped = 0;
        for(i=1; i<lens; i++){
            if(freqs[i-1] > freqs[i]){
                //swap chars
                ctemp=cs[i];
                cs[i]=cs[i-1];
                cs[i-1]=ctemp;

                //swap freqs
                itemp=freqs[i];
                freqs[i]=freqs[i-1];
                freqs[i-1]=itemp;

                swapped = 1;
            }
        }
        len--;
    }while(swapped==1);

}
