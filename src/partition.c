#include<stdio.h>
#include<stdint.h>
#include"partition.h"
#include"disk.h"

int partition() {
    // System is LITTLE endian
    // Begin superblock
    struct block super;
    // Begin magic number
    super.block[3] = 0xf0;
    super.block[2] = 0xf0;
    super.block[1] = 0x34;
    super.block[0] = 0x10;

    // Begin total number of blocks
    super.block[7] = 0x00;
    super.block[6] = 0x00;
    super.block[5] = 0x08;
    super.block[4] = 0x00;

    // Begin total number of inodes
    super.block[11] = 0x00;
    super.block[10] = 0x00;
    super.block[9] = 0x00;
    super.block[8] = 0xCD;
    
    disk_write(super);

    // Begin INode Bitmap
    struct block ibitmap;
    ibitmap.block

    

    //printf("%x %x %x", new.fsid, new.blocknumber, new.inodes);

    return 0;
}