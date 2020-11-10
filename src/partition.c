#include<stdio.h>
#include"disk.h"
#include"partition.h"

int partition() {
    // System is LITTLE endian
    // Begin superblock
    struct block superblock;
    superblock.block[3] = 0xf0;
    superblock.block[2] = 0xf0;
    superblock.block[1] = 0x34;
    superblock.block[0] = 0x10;

    superblock.block[7] = 0x00;
    superblock.block[6] = 0x00;
    superblock.block[5] = 0x07;
    superblock.block[4] = 0xD0;

    superblock.block[11] = 0x00;
    superblock.block[10] = 0x00;
    superblock.block[9] = 0x07;
    superblock.block[8] = 0xD0;

    return 0;
    /*for (int i = 0; i < 4; i++) {
        printf("%02x ", superblock.block[i]);
    }*/

    //disk_write()
}