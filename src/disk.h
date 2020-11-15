#ifndef DISK_H_
#define DISK_H_

#define BLOCK_SIZE 128
#define MAX_RECORDS 2056 

struct block {
    unsigned char block[BLOCK_SIZE];
};

int disk_write(struct block data);
int disk_read(int record, struct block* passback);

#endif