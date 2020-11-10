#include<stdio.h>
#include<stdlib.h>
#include"disk.h"
#include"partition.h"

int main() {
    partition();

    /*struct block block1;

    for (int i = 0; i < BLOCK_SIZE; i++) {
        block1.block[i] = 0xAA;
    }

    int pos = disk_write(block1);

    struct block read;

    disk_read(pos, &read);

    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02x ", read.block[i]);
    }*/
}