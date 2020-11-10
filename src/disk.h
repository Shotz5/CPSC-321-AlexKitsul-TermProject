struct block {
    char block[128];
};

int main();
int disk();
int disk_write(struct block data);
int disk_read(struct block data);
