#ifndef KERNEL_IO_H
#define KERNEL_IO_H

unsigned char insb(unsigned short port);

unsigned short insw(unsigned short port);

void outb(unsigned short port, unsigned char value);
void outw(unsigned short port, unsigned short value);

#endif //KERNEL_IO_H