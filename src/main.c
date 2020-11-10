#include<stdio.h>
#include<stdlib.h>
#include"disk.h"


int main() {
    struct block block1;

    for (int i = 0; i < 128; i++) {
        block1.block[i] = 'a';
    }
}