#ifndef DISK_H_
#define DISK_H_

#define BLOCK_SIZE 128
#define MAX_RECORDS 2056

struct block {
    unsigned char block[BLOCK_SIZE];
};

struct superblock {
    uint32_t fsid;
    uint32_t blocknumber;
    uint32_t inodes;
};

int disk_write(struct block data);
int disk_read(int record, struct block* passback);


#endif