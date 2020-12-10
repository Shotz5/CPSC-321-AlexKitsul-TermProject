#include"main.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// 0 - superblock
// 1 - bitmap
// 2 - 411 - inodes
// 412 - first data bitmap
// 413 - 1437 - first data group

block disk[TOTAL_BLOCKS];
int numfiles = 0;

int main() {
    partition();
    make_file();
    write_file("Pklfjsdklfjklsdjfkllllllllllllllllllllllllllllllllllllllllllllsdafsdafsdklfjsdklfjklsdjfklsdjfsd;fkjsdl;fkjlf;", 0);
}

int partition() {
    setsuper();
    setbitmap();
    setdatabitmap();
}

int setsuper() {
    block superblock;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        superblock.data[i] = 0x00;
    }
    hex_to_char(FS, &superblock, 0);
    hex_to_char(TOTAL_BLOCKS_HEX, &superblock, 4);
    hex_to_char(TOTAL_INODES_HEX, &superblock, 8);
    disk_write(superblock, 0);
    return 0;
}

int setbitmap() {
    block inodebitmap;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        inodebitmap.data[i] = 0x00;
    }
    disk_write(inodebitmap, 1);
    return 0;
}

int setdatabitmap() {
    block databitmap;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        databitmap.data[i] = 0x00;
    }
    disk_write(databitmap, (TOTAL_INODES + 2));
    return 0;
}

int make_file() {
    bitlocation bits;
    bits = find_bit(1);
    int inodelocation = (bits.i + bits.j);
    // Make inode block
    block inode;
    // File size set to 0
    hex_to_char(0x00000000, &inode, 0);
    // Direct pointers set to 0
    for (int i = 1; i < 5; i++) {
        hex_to_char(0x00000000, &inode, i * 4);
    }
    // Skip indirect pointers for now

    disk_write(inode, inodelocation + 2);
    printf("New file inode number: %d \n", inodelocation);
}

int write_file(char* content, int inodelocation) {
    bitlocation bits[4];
    for (int i = 0; i < 4; i++) {
        bits[i].i = -1;
        bits[i].j = -1;
    }

    int bitmapblock[4];
    int foundbits = 0;
    int bitmap = 0;

    while (foundbits != 4) {
        bits[foundbits] = find_bit(TOTAL_DATA_BITMAP_BLOCKS);
        if (bits[foundbits].i == -1) {
            bitmap++;
        } else {
            bitmapblock[foundbits] = bitmap;
            foundbits++;
        }
    }

    block* inode;
    inode = disk_read(inodelocation);
    int filesize = (strlen(content) + 1);
    int pointers[foundbits];
    hex_to_char(filesize, inode, 0);
    for (int i = 0; i < foundbits; i++) {
        pointers[i] = 2 + TOTAL_INODES + (1025 * bitmapblock[i]) + bits[i].i + bits[i].j;
        hex_to_char(pointers[i], inode, ((i + 1) * 4));
    }

    block datablock;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        datablock.data[i] = 0x00;
    }

    int datablockindex = 0;
    for (int i = 0; i < (strlen(content) + 1); i++) {
        if (i % 128 == 0 && i != 0) {
            datablockindex++;
            disk_write(datablock, pointers[datablockindex]);
            initializedatablock(&datablock);
        }
        datablock.data[i % 128] = content[i];
    }
    disk_write(datablock, pointers[datablockindex]);
}

int delete_file(int inodedeletion) {
    
} 

int initializedatablock(block* datablock) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        datablock->data[i] = 0x00;
    }
}

bitlocation find_bit(int bitmaplocation) {
    block* bitmap = disk_read(bitmaplocation);
    bitlocation bits;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 7; j >=0; j--) {
            if (get_bit(bitmap->data[i], j) == 0) {
                set_bit(&bitmap->data[i], j);
                bits.i = i;
                bits.j = (7 - j);
                return bits;
            }
        }
    }
    return bits;
}

block* disk_read(int blockindex) {
    if (blockindex < 0 || blockindex > TOTAL_BLOCKS) {
        return NULL;
    } else {
        return &disk[blockindex];
    }
}

int disk_write(block string, int blockindex) {
    if (blockindex < 0 || blockindex > TOTAL_BLOCKS) {
        return -1;
    } else {
        disk[blockindex] = string;
    }
}

