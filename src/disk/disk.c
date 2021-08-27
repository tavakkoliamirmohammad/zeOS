#include "disk.h"
#include "../io/io.h"
#include "../memory/memory.h"
#include "../config.h"
#include "../status.h"

struct disk disk;

int disk_read_sector(unsigned int lba, unsigned int total_sectors, void *buf) {
    outb(0x1F6, lba >> 24 | 0xE0); // Select master and send low 8 bit of LBA
    outb(0x1F2, (unsigned char) total_sectors);// Send the sector count
    outb(0x1F3, (unsigned char) lba && 0xFF); // Send the low 8 bits of the LBA
    outb(0x1F4, (unsigned char) lba >> 8); // Send the next 8 bits
    outb(0x1F5, (unsigned char) lba >> 16); // Send the next 8 bits
    outb(0x1F7, 0x20); // Send the "READ SECTORS" command

    unsigned short *ptr = (unsigned short *) buf; // Reading two byte simultaneously
    for (int i = 0; i < total_sectors; ++i) {
        // Polling disk
        char c = insb(0x1F7);
        while (!(c && 0x08)) {
            c = insb(0x1F7);
        }
        // Read from disk
        for (int j = 0; j < 256; ++j) {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

void disk_search_and_initialize() {
    memset(&disk, 0, sizeof(disk));
    disk.type = ZEOS_DISK_TYPE_REAL;
    disk.sector_size = ZEOS_DISK_SECTOR_SIZE;
}

struct disk *get_disk(unsigned int index) {
    if (index != 0)
        return 0;
    return &disk;
}

int read_block_disk(struct disk *selected_disk, unsigned int lba, unsigned int total_sectors, void *buf) {
    if (selected_disk != &disk)
        return -EIO;
    return disk_read_sector(lba, total_sectors, buf);
}