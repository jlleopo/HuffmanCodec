#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIGNUM 1000

typedef struct node{
  char symb;
  int freq;
  struct node *left;
  struct node *right;
} t_node;

void compress(FILE *fptIn, FILE *fptOut);
int containedIn(unsigned char *list, unsigned char c, int *i);
void mSort(unsigned char *cs, int *freqs);
void sortTrees(t_node **trees, int len);
void formCodes(t_node *node, char *currentCode, char **codeList, unsigned char *symbolList, int *i);
void writeOutCodes(FILE *fptIn, FILE *fptOut, char **codeList, int numCodes, unsigned char *symbolList);
unsigned char makeBitmask(int length);


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
    unsigned char *allSymbols;   //list of symbols found in fptIn
    int *freqs;         //frequencies of said symbols
    unsigned char c;
    int i;
    int len;
    char *currentCode;
    char **codeList;
    unsigned char *symbolList;

    t_node **ptrees;
    t_node *newNode;
    //t_node ftree;

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
    mSort(allSymbols, freqs);

    while(freqs[i]!=0) i++;
    len = i;

    ptrees = (t_node **) calloc(len, sizeof(t_node *));
    for(i=0; i<len; i++){
        ptrees[i] = (t_node *) malloc(sizeof(t_node));

        ptrees[i]->symb = allSymbols[i];
        ptrees[i]->freq = freqs[i];
        ptrees[i]->left = NULL;
        ptrees[i]->right = NULL;
    }

    while(ptrees[1]!=NULL){
        newNode = (t_node *) malloc(sizeof(t_node));

        newNode->left = ptrees[0];
        newNode->right = ptrees[1];
        newNode->freq = newNode->left->freq + newNode->right->freq;
        newNode->symb = 0;

        ptrees[0] = newNode;
        ptrees[1] = NULL;

        //sort
        sortTrees(ptrees, len);
    }

    codeList = (char **) calloc(len, sizeof(char *));
    currentCode = (char *) calloc(100, sizeof(char));
    symbolList = (unsigned char *) calloc(len, sizeof(unsigned char));
    i=0;
    formCodes(ptrees[0], currentCode, codeList, symbolList, &i);

    writeOutCodes(fptIn, fptOut, codeList, len, symbolList);

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

//sorts cs by freqs in ascending order
//tested: works
void mSort(unsigned char *cs, int *freqs){
    int i;
    int len=0;
    int swapped;
    char ctemp;
    int itemp;

    while(freqs[len]!=0) len++;

    do{
        swapped = 0;
        for(i=1; i<len; i++){
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

//sorts a list of tree nodes based on the freqs of their roots
void sortTrees(t_node **trees, int len){
    int swapped;
    t_node *temp;
    int i;

    //how the fuck do we sort nulls
    do{
        swapped = 0;
        for(i=1; i<len; i++){
            if(trees[i] == NULL){
                //do nothing
            }else if(trees[i-1] == NULL || trees[i-1]->freq > trees[i]->freq){
                //swap
                temp = trees[i-1];
                trees[i-1] = trees[i];
                trees[i] = temp;

                swapped = 1;
            }
        }
        len--;
    }while(swapped==1);

}

//if we go left add a 0
//if we go right add a 1
//
//I don't think there's any way we could have a node on only one side
void formCodes(t_node *node, char *currentCode, char **codeList, unsigned char *symbolList, int *i){
    if(node->left == NULL && node->right == NULL){
        codeList[*i] = (char *) malloc(sizeof(char)*100);
        strcpy(codeList[*i], currentCode);
        symbolList[*i]=node->symb;
        (*i)++;
    } else {
        //add a 1
        int len = strlen(currentCode);
        currentCode[len] = '1';
        currentCode[len+1] = 0x0;
        //go right
        formCodes(node->right, currentCode, codeList, symbolList, i);

        //add a 0
        currentCode[len] = '0';
        currentCode[len+1] = 0x0;
        //go left
        formCodes(node->left, currentCode, codeList, symbolList, i);
    }
}

void writeOutCodes(FILE *fptIn, FILE *fptOut, char **codeList, int numCodes, unsigned char *symbolList){
    int *lengths;
    int i;
    int position;
    unsigned char c; //input byte
    unsigned char b; //output byte
    unsigned char mask;
    unsigned char *bCodeList;

    lengths = (int *) calloc(numCodes, sizeof(int));
    bCodeList = (unsigned char *) calloc(numCodes, sizeof(unsigned char));
    for(i=0; i<numCodes; i++){
        lengths[i] = strlen(codeList[i]);
        bCodeList[i] = (char) strtol(codeList[i], NULL, 2);
    }

    position = 8;
    while(fread(&c, 1, 1, fptIn) == 1){
        for(i=0; i<numCodes; i++){
            if(symbolList[i]==c) break;
        }

        //make bitmask lengths[i] long
        mask = makeBitmask(lengths[i]); // might not need this  yolo

        if(position > lengths[i]){
            //we have enough space to fit all of our pattern in the output byte
            b |= (bCodeList[i] << position);
            position -= lengths[i];
        } else {
            b |= (bCodeList[i] >> (position - lengths[i]));
            fwrite(&b, 1, 1, fptOut);
            b = 0x0;

            b |= (bCodeList[i] >> (8 - lengths[i] + position));
            position = 8 - lengths[i] + position;
        }



    }
}

unsigned char makeBitmask(int length){
    int i;
    char c=0;

    for(i=0; i<length; i++){
        c &= 1<<i;
    }
    return c;
}

void decompress(FILE *fptIn, FILE *fptOut){
    int i;

    for(i=0;i<1000; i++) printf("I forgot to do this lol\t");
}
