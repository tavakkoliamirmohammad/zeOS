#include "paging.h"
#include "../heap/kheap.h"

void paging_load_directory(paging_table_entry *directory);

struct paging_4gb_chunk *new_paging_4gb_chunk(uint8_t flags) {
    paging_table_entry *paging_directory_entry = kzalloc(
            PAGING_TOTAL_ENTRIES_PER_TABLE * sizeof(paging_table_entry));
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; ++i) {
        paging_table_entry *paging_entry = kzalloc(PAGING_TOTAL_ENTRIES_PER_TABLE * sizeof(paging_table_entry));
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; ++j) {
            paging_entry[j] = (offset + j * PAGING_ENTRIES_SIZE) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_ENTRIES_SIZE);
        paging_directory_entry[i] = (paging_table_entry) paging_entry | flags | PAGING_ACCESS_WRITABLE;
    }
    struct paging_4gb_chunk *paging4GbChunk = kzalloc(sizeof(struct paging_4gb_chunk));
    paging4GbChunk->directoryEntry = paging_directory_entry;
    return paging4GbChunk;
}

paging_table_entry *paging_4gb_chunk_get_directory(struct paging_4gb_chunk *chunk) {
    return chunk->directoryEntry;
}

void paging_switch_directory(paging_table_entry *directory) {
    paging_load_directory(directory);
}