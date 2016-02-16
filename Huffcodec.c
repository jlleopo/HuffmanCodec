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
unsigned int makeBitmask(int length);
unsigned long int calculateNumBits(int *lengths, unsigned char *symbolList, FILE *fptIn);


void decompress(FILE *fptIn, FILE *fptOut);
int inCodeList(int code, int codeLen, unsigned int *codeList, int *lengths, int numCodes);

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
    int *freqs;         //frequencies of symbols
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
    int bitsLeft;
    unsigned char c; //input byte
    unsigned char b = 0; //output byte
    unsigned int mask;
    unsigned int *bCodeList;
    unsigned long int totalNumBits;

    lengths = (int *) calloc(numCodes, sizeof(int));
    bCodeList = (unsigned int *) calloc(numCodes, sizeof(unsigned int));
    for(i=0; i<numCodes; i++){
        lengths[i] = strlen(codeList[i]);
        bCodeList[i] = (int) strtol(codeList[i], NULL, 2);
    }



    //write out a header
    fwrite(&numCodes, sizeof(int), 1, fptOut);
    //write out number of bits
    totalNumBits = calculateNumBits(lengths, symbolList, fptIn);
    fwrite(&totalNumBits, sizeof(long int), 1, fptOut);
    for(i=0; i<numCodes; i++){
        fwrite((symbolList+i), sizeof(unsigned char), 1, fptOut);
        fwrite((lengths+i), sizeof(int), 1, fptOut);
        fwrite((bCodeList+i), sizeof(unsigned int), 1, fptOut);
    }

    fptIn = freopen(NULL, "rb", fptIn);
    position = 8;
    while(fread(&c, 1, 1, fptIn) == 1){
        for(i=0; i<numCodes; i++){
            if(symbolList[i]==c) break;
        }



        if(position > lengths[i]){
            //we have enough space to fit all of our pattern in the output byte
            b |= (bCodeList[i] << (position - lengths[i]));
            position -= lengths[i];
        } else if(position == lengths[i]){
                mask = makeBitmask(lengths[i]);

                b |= bCodeList[i] & mask;
                fwrite(&b, 1, 1, fptOut);
                b = 0x0;
                position = 8;
        } else {
            //put all that we can in this byte before we write it out
            b |= (bCodeList[i] >> (lengths[i] - position));
            fwrite(&b, 1, 1, fptOut);

            //start with new byte
            b = 0x0;

            //bits we have left to write
            //always going to be total number (length) minus
            //number we just wrote (position)
            bitsLeft = lengths[i] - position;
            while(bitsLeft > 0){
                if(bitsLeft > 8){
                    mask = makeBitmask(8);

                    //write out 8 bits and start with a new byte
                    b |= (bCodeList[i] >> (bitsLeft - 8));
                    fwrite(&b, 1, 1, fptOut);
                    b = 0x0;
                    //new position
                    bitsLeft -= 8;
                    position = 8;
                } else {
                    b |= (bCodeList[i] << (8 - bitsLeft));
                    position = 8 - bitsLeft;
                    bitsLeft = 0;
                }
            }
        }



    }
    fwrite(&b, 1, 1, fptOut);
}

unsigned int makeBitmask(int length){
    int i;
    int c=0;

    for(i=0; i<length; i++){
        c |= 1<<i;
    }
    return c;
}

unsigned long int calculateNumBits(int *lengths, unsigned char *symbolList, FILE *fptIn){
    unsigned char c;
    unsigned long int count = 0;
    int i;

    fptIn = freopen(NULL, "rb", fptIn);
    while(fread(&c, 1, 1, fptIn) == 1){
        for(i=0; lengths[i]!=0; i++){
            if(symbolList[i] == c) break;
        }
        count += lengths[i];
    }

    return count;
}

void decompress(FILE *fptIn, FILE *fptOut){
    int i;
    int *lengths;
    unsigned int *codeList;
    unsigned char *symbolList;
    int numCodes;
    char c;
    int code;
    int codeLen;
    //int mask;
    unsigned long int totalNumBits;
    unsigned long int currNumBits=0;
    int position;
    int index;
    int maxLen = 0;

    //read in the header
    fread(&numCodes, sizeof(int), 1, fptIn);
    fread(&totalNumBits, sizeof(long int), 1, fptIn);
    symbolList = (unsigned char *) calloc(numCodes, sizeof(unsigned char));
    lengths = (int *) calloc(numCodes, sizeof(int));
    codeList = (unsigned int *) calloc(numCodes, sizeof(unsigned int));
    for(i=0; i<numCodes; i++){
        fread((symbolList+i), sizeof(unsigned char), 1, fptIn);
        fread((lengths+i), sizeof(int), 1, fptIn);
        if(lengths[i] > maxLen) maxLen = lengths[i];
        fread((codeList+i), sizeof(unsigned int), 1, fptIn);
    }

    //shiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiit
    fread(&c, 1, 1, fptIn);
    position = 8;
    while(currNumBits < totalNumBits){
            //grab one bit at a time
            //shift our entire current code 1 bit to the left
            //add that bit to the start of our code
            //check if it's in the dictionary
            code = 0;
            codeLen = 0;
            do{
                code <<= 1;
                code |= (c & (1 << (position-1))) >> (position-1);
                position --;
                currNumBits++;
                codeLen++;

                if(position == 0){
                    fread(&c, 1, 1, fptIn);
                    position = 8;
                }
            } while((index = inCodeList(code, codeLen, codeList, lengths, numCodes)) < 0);


            fwrite((symbolList+index), 1, 1, fptOut);
    }

}

//returns index of code if in list
//returns -1 otherwise
int inCodeList(int code, int codeLen, unsigned int *codeList, int *lengths, int numCodes){
    int i;
    for(i=0;i<numCodes;i++){
        if(codeLen==lengths[i] && codeList[i]==code) return i;
    }
    return -1;
}
