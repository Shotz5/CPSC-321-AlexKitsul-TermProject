#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"disk.h"

struct block records[MAX_RECORDS]; // Big bulk of blocks
int recordpos = 0; // Currently selected block

// Writes to our virtual disk which is this case is just RAM
// Returns record index that was written to, or -1 for invalid record
int disk_write(struct block data) {
    if (recordpos < 0 || recordpos >= MAX_RECORDS) {
        printf("Wrote outside of record limit");
        return -1;
    }
    records[recordpos] = data;
    return recordpos++;
}

// Reads from our virutal disk
// Returns int, -1 on error, 0 on success
int disk_read(int record, struct block* passback) {
    if (record < 0 || record >= MAX_RECORDS) {
        printf("Read outside of record limit");
        return -1;
    }
    *passback = records[record];
    return 0;
}

/*int disk_read(struct superblock* passback) {
    *passback = records[0];
    return 0;
}*/