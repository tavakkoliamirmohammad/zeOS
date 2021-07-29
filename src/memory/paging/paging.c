#include "paging.h"
#include "../heap/kheap.h"
#include "../../status.h"

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

bool paging_is_address_aligned(void *ptr) {
    return ((uint32_t) ptr % PAGING_ENTRIES_SIZE) == 0
}

int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out) {
    if (!paging_is_address_aligned(virtual_address)) {
        return -EINVARG;
    }
    *directory_index_out = ((uint32_t) virtual_address / (PAGING_ENTRIES_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE));
    *table_index_out =
            ((uint32_t) virtual_address % (PAGING_ENTRIES_SIZE * PAGING_TOTAL_ENTRIES_PER_TABLE)) / PAGING_ENTRIES_SIZE;
    return 0;
}

int paging_set(paging_table_entry *directory, void *virtual_address, uint32_t value) {
    if (!paging_is_address_aligned(virtual_address)) {
        return -EINVARG;
    }

    uint32_t directory_index_out = 0, table_index_out = 0;
    int res = paging_get_indexes(virtual_address, &directory_index_out, &table_index_out);
    if (res < 0)
        return res;
    paging_table_entry entry = directory[directory_index_out];
    paging_table_entry *entry_table_address = (paging_table_entry *) (entry & 0xFFFFF000);
    entry_table_address[table_index_out] = value;
    return 0;
}