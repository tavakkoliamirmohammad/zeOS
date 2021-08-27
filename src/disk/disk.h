#ifndef KERNEL_DISK_H
#define KERNEL_DISK_H

int disk_read_sector(int lba, int total_sectors, void *buf);

#endif //KERNEL_DISK_H
