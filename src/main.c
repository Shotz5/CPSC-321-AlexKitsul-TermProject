#include<stdio.h>
#include<stdlib.h>
#include"disk.h"

int main() {
    struct block block1;

    for (int i = 0; i < BLOCK_SIZE; i++) {
        block1.block[i] = 0xAA;
    }

    int pos = disk_write(block1);

    struct block read = disk_read(pos);

    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02x ", read.block[i]);
    }

    for (int i = 0; i < BLOCK_SIZE; i++) {
        block1.block[i] = 0xBB;
    }

    pos = disk_write(block1);

    read = disk_read(pos);

    for (int i = 0; i < BLOCK_SIZE; i++) {
        printf("%02x ", read.block[i]);
    }
}