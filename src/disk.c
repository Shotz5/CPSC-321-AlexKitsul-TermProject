#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"disk.h"

/* Partitions our hard drive with all the necessary supers and bitmaps */
int partition() {
    setsuper();
    setbitmap();
    setdatabitmap();
}

/* 0 out the garbage data in the superblock and append our constants */
int setsuper() {
    block superblock;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        superblock.data[i] = 0x00;
    }
    hex_to_char(FS, &superblock, 0);
    hex_to_char(TOTAL_BLOCKS_HEX, &superblock, 4);
    hex_to_char(TOTAL_INODES_HEX, &superblock, 8);
    disk_write(superblock, SUPERBLOCK_LOC);
    return 0;
}

/* 0 out the inode bitmap and append to our disk */
int setbitmap() {
    block inodebitmap;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        inodebitmap.data[i] = 0x00;
    }
    disk_write(inodebitmap, INODE_BITMAP_LOC);
    return 0;
}

/* 0 out the data bitmap and append to our disk */
int setdatabitmap() {
    block databitmap;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        databitmap.data[i] = 0x00;
    }
    disk_write(databitmap, (TOTAL_INODES + 2));
    return 0;
}

/* Constuct and allocate inode to file and return inode position, print this to the user */
int make_file() {
    // Uses some math to find the bit location of the inode we want to grab
    bitlocation bits;
    bits = find_bit(INODE_BITMAP_LOC);
    int inodelocation = ((bits.i * 8) + bits.j);

    if (inodelocation > MAX_FILES) {
        printf("Out of inodes!");
        return -1;
    }

    // Make inode block
    block inode;
    // File size set to 0
    hex_to_char(0x00000000, &inode, 0);
    // Direct pointers set to 0
    for (int i = 1; i < 5; i++) {
        hex_to_char(0x00000000, &inode, i * 4);
    }
    // Skip indirect pointers for now (never got around to implementing, it's 11pm as I'm typing this)
    // Write inode to it's location on the drive
    disk_write(inode, inodelocation + 2);
    printf("New file inode number: %d \n", inodelocation);
}

/* Write a string to the file */
int write_file(char* content, int inodelocation) {
    // If content is too big
    if (strlen(content) > BLOCK_SIZE * 4) {
        printf("Data too big \n");
        return -1;
    }

    // For 4 direct block pointers we need 4 inodebitmap locations
    bitlocation bits[4];
    for (int i = 0; i < 4; i++) {
        bits[i].i = -1;
        bits[i].j = -1;
    }

    // Block that the available data block was found
    int bitmapblock[4];
    // Open data blocks pointer
    int foundbits = 0;
    // Data block bitmap lookup
    int bitmap = TOTAL_INODES + 2;

    // Iterates until it finds 4 open data block locations for the direct pointers
    while (foundbits != 4) {
        bits[foundbits] = find_bit(bitmap);
        // If we were unable to find any data blocks in the current databitmap
        if (bits[foundbits].i == -1) {
            bitmap += 1024;
        } else {
            bitmapblock[foundbits] = (bitmap - TOTAL_INODES - 2);
            foundbits++;
        }
    }

    // inodes all used up
    if (inodelocation > MAX_FILES) {
        printf("Too many files, not enough inodes.\n");
        return -1;
    }

    // Read the inode from its provided location
    block* inode;
    inode = disk_read(inodelocation + 2);
    // Update the file size
    int filesize = (strlen(content) + 1);
    hex_to_char(filesize, inode, 0);
    // Initialize pointer locations array
    int pointers[foundbits];
    
    // Iterate through the open found bits and 
    for (int i = 0; i < foundbits; i++) {
        pointers[i] = 2 + TOTAL_INODES + (1024 * bitmapblock[i]) + bits[i].i + bits[i].j;
        hex_to_char(pointers[i], inode, ((i + 1) * 4));
    }

    // Build the data block
    block datablock;
    initializedatablock(&datablock);
    
    // Logic for placing the data inside multiple data blocks if the amount of data exceeds 1 block
    int datablockindex = 0;
    for (int i = 0; i < (strlen(content) + 1); i++) {
        if (i % BLOCK_SIZE == 0 && i != 0) {
            disk_write(datablock, pointers[datablockindex]);
            datablockindex++;
            initializedatablock(&datablock);
        }
        datablock.data[i % 128] = content[i];
    }
    disk_write(datablock, pointers[datablockindex]);

    // Writes hex values written to disk in console
    printf("Wrote raw data: ");
    for (int i = 0; i < 4; i++) {
        block* data;
        data = disk_read(pointers[i]);

        for (int j = 0; j < BLOCK_SIZE; j++) {
            if (data->data[j] == 0x00) {
                printf("\n");
                return 0;
            }
            printf("%02x ", data->data[j]);
        }
    }
    printf("\n");
}

/* Method for deleting a file by 0ing out its inode and its inode bitmap */
int delete_file(int inodeindex) {
    // Find the inode and pull the bitmap
    block* inodebitmap;
    inodebitmap = disk_read(INODE_BITMAP_LOC);
    block* inode;
    inode = disk_read(inodeindex + 2);

    // Math that finds the bitmap location from the inode's index
    bitlocation bits;
    bits.i = (inodeindex / 8);
    bits.j = (inodeindex % 8);

    // Lower bit from 1 - 0
    lower_bit(&inodebitmap->data[bits.i], (7 - bits.j));

    // 0 out the inode
    for (int i = 0; i < BLOCK_SIZE; i++) {
        inode->data[i] = 0x00;
    }
    return 0;
}

/* 0 the data block to be written to in case garbage data shows up */
int initializedatablock(block* datablock) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        datablock->data[i] = 0x00;
    }
}

/* Parse a block bitmap to find a bit that equals 0 and returns its location */
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

// Sourced from https://stackoverflow.com/questions/22004798/how-to-access-a-specific-bit-given-a-byte-in-c
/* Gets a bit at a certain position using bit shifting */
int get_bit(char bits, char pos) {
    return (bits >> pos) & 0x01;
}

// Sourced from https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
// Sets a bit at a certain position to 1
int set_bit(char* bits, char pos) {
    *bits |= 1 << pos;
    return 0;
}
// Sets a bit at a certain position to 0
int lower_bit(char* bits, char pos) {
    *bits &= ~(1 << pos);
    return 0;
}

// Sourced from https://stackoverflow.com/questions/3784263/converting-an-int-into-a-4-byte-char-array-c
// Converts a 32 bit hex value to 4 sequential chars at the offset
int hex_to_char(unsigned int hex, block* modify, int offset) {
    for (int i = 3; i >= 0; i--) {
        modify->data[(3 - i) + offset] = ((hex >> (i * 8)) & 0xFF);
    }
    return 0;
}

/* Reads a block at blockindex from the disk */
block* disk_read(int blockindex) {
    if (blockindex < 0 || blockindex > TOTAL_BLOCKS) {
        return NULL;
    } else {
        return &disk[blockindex];
    }
}

/* Write a block at blockindex to the disk */
int disk_write(block string, int blockindex) {
    if (blockindex < 0 || blockindex > TOTAL_BLOCKS) {
        return -1;
    } else {
        disk[blockindex] = string;
    }
}

