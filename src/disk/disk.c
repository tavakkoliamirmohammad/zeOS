#include "disk.h"
#include "../io/io.h"

int disk_read_sector(int lba, int total_sectors, void *buf) {
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