#include<stdio.h>
#include<stdlib.h>
#include"disk.h"

struct block records[2000];
int recordpos = 0;

int disk_write(struct block data) {
    records[recordpos] = data;
    return recordpos++;
}

struct block disk_read(int record) {
    return records[record];
}