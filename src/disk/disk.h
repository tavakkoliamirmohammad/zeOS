#ifndef KERNEL_DISK_H
#define KERNEL_DISK_H

typedef unsigned int ZEOS_DISK_TYPE;

#define ZEOS_DISK_TYPE_REAL 0

struct disk {
    ZEOS_DISK_TYPE type;
    unsigned int sector_size;
};

void disk_search_and_initialize();

struct disk *get_disk(unsigned int index);

int read_block_disk(struct disk *selected_disk, unsigned int lba, unsigned int total_sectors, void *buf);

#endif //KERNEL_DISK_H
