#define BLOCK_SIZE 128

struct block {
    unsigned char block[BLOCK_SIZE];
};

int disk_write(struct block data);
struct block disk_read(int record);
