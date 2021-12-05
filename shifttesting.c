#include <stdio.h>
#include <stdlib.h>

int * binaryNumber(unsigned int num) {
    int *binaryArray = (int*)calloc(64, sizeof(int));
    for(int i = 63; i >= 0; i--) {
        binaryArray[i] = num%2;
        num = num/2;
    }
    return binaryArray;
}



int main(int argc, char **argv) {
    unsigned int hex = 0xff00039c;
    unsigned int shiftright = hex >> 4;
    fprintf(stdout, "shiftright = %x\n", shiftright);
    unsigned int shiftleft = shiftright << 4;
    fprintf(stdout, "shiftleft = %x\n", shiftleft);
    int difference = hex - shiftleft;
    fprintf(stdout, "difference = %d\n", difference);

    int *p_int = binaryNumber(hex);
    fprintf(stdout, "number as hex = %x\n", hex);
    fprintf(stdout, "number as unsigned int = %u\n", hex);
    fprintf(stdout, "number as binary = ");
    for(int i = 0; i < 64; i++) {
        printf("%d", p_int[i]);
        if((i+1)%4 == 0) {
            printf(" ");
        }
    }
    printf("\n");
    free(p_int);
    return 0;
}
