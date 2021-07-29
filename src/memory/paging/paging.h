#ifndef KERNEL_PAGING_H
#define KERNEL_PAGING_H

#include <stddef.h>
#include <stdint.h>

#define PAGING_CACHE_DISABLED      0b00010000
#define PAGING_CACHE_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_ALL          0b00000100
#define PAGING_ACCESS_WRITABLE   0b00000010
#define PAGING_PRESENT             0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_ENTRIES_SIZE 4096

typedef uint32_t paging_table_entry;

struct paging_4gb_chunk {
    paging_table_entry *directoryEntry;
};

struct paging_4gb_chunk *new_paging_4gb_chunk(uint8_t flags);

paging_table_entry *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk);

void paging_switch_directory(paging_table_entry *directory);

void enable_paging();


#endif //KERNEL_PAGING_H
