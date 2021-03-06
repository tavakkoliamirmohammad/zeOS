#ifndef KERNEL_KHEAP_H
#define KERNEL_KHEAP_H

#include <stdint.h>
#include <stddef.h>
#include "heap.h"


void kheap_init();

void *kmalloc(size_t size);

void kfree(void *ptr);

void *kzalloc(size_t size);

#endif //KERNEL_KHEAP_H
