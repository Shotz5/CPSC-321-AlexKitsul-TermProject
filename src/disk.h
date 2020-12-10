#ifndef DISK_H
#define DISK_H

#define BLOCK_SIZE 128
#define DATA_GROUP 1024
#define TOTAL_DATA_BLOCKS 4096
#define TOTAL_INODES 410
#define TOTAL_BLOCKS 4508
#define TOTAL_DATA_BITMAP_BLOCKS 4
#define FIRST_DATA_BLOCK 413
#define POINTERS_PER_INODE 4
#define INODE_BITMAP_LOC 1
#define SUPERBLOCK_LOC 0

#define FS 0xf0f03410
#define TOTAL_BLOCKS_HEX 0x00001000
#define TOTAL_INODES_HEX 0x0000019A
#define MAX_FILES 410

// Main block typedef for the disk consisting of 128 byte blocks
typedef struct {
    unsigned char data[BLOCK_SIZE];
} block;

// Struct for finding bit locations and using bit shifting (mainly just so I could return two numbers in one return)
typedef struct {
    int i;
    int j;
} bitlocation;

// Main hard drive (I thought it would look more shiny too...)
block disk[TOTAL_BLOCKS];

// Method declarations, descriptions for all in .c file
int get_bit(char bits, char pos);
int set_bit(char* bits, char pos);
int hex_to_char(unsigned int hex, block* modify, int offset);
int lower_bit(char* bits, char pos);
int main();
int setsuper();
int setbitmap();
int setdatabitmap();
int make_file();
int partition();
int delete_file(int inodeindex);
int write_file(char* content, int inodelocation);
int initializedatablock(block* datablock);
bitlocation find_bit(int bitmaplocation);
block* disk_read(int blockindex);
int disk_write(block string, int blockindex);

#endif