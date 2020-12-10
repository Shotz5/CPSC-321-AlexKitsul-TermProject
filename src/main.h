#ifndef _MAIN_H
#define _MAIN_H

#define BLOCK_SIZE 128
#define DATA_GROUP 1024
#define TOTAL_DATA_BLOCKS 4096
#define TOTAL_INODES 410
#define TOTAL_BLOCKS 4508
#define TOTAL_DATA_BITMAP_BLOCKS 4
#define FIRST_DATA_BLOCK 413

#define FS 0xf0f03410
#define TOTAL_BLOCKS_HEX 0x00001000
#define TOTAL_INODES_HEX 0x0000019A
#define MAX_FILES 410

typedef struct {
    unsigned char data[BLOCK_SIZE];
    int pointer;
} block;

typedef struct {
    int i;
    int j;
} bitlocation;

// Sourced from https://stackoverflow.com/questions/22004798/how-to-access-a-specific-bit-given-a-byte-in-c
int get_bit(char bits, char pos) {
    return (bits >> pos) & 0x01;
}
int set_bit(char* bits, char pos) {
    *bits |= 1 << pos;
    return 0;
}

// Sourced from https://stackoverflow.com/questions/3784263/converting-an-int-into-a-4-byte-char-array-c
int hex_to_char(unsigned int hex, block* modify, int offset) {
    for (int i = 3; i >= 0; i--) {
        modify->data[(3 - i) + offset] = ((hex >> (i * 8)) & 0xFF);
    }
    return 0;
}

int main();
int setsuper();
int setbitmap();
int setdatabitmap();
int make_file();
int write_file(char* content, int inodelocation);
int initializedatablock(block* datablock);
bitlocation find_bit(int bitmaplocation);
block* disk_read(int blockindex);
int disk_write(block string, int blockindex);

#endif