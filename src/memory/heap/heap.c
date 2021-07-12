#include "heap.h"
#include "../../status.h"
#include "../memory.h"

static int heap_validate_alignment(void *ptr) {
    return ((unsigned int) ptr % ZEOS_HEAP_BLOCK_SIZE) == 0;
}

static int heap_validate_table(void *start, void *end, struct heap_table *heapTable) {
    size_t table_size = (size_t)(end - start);
    size_t total_pages = table_size / ZEOS_HEAP_BLOCK_SIZE;
    if (total_pages != heapTable->total) {
        return -EINVARG;
    }
    return ZEROS_ALL_OK;
}

int heap_create(struct heap *heap, void *start, void *end, struct heap_table *heapTable) {
    int res = ZEROS_ALL_OK;
    if (!heap_validate_alignment(start) || !heap_validate_alignment(end)) {
        return -EINVARG;
    }
    memset(heap, 0, sizeof(struct heap));
    heap->start_address = start;
    heap->heapTable = heapTable;

    res = heap_validate_table(start, end, heapTable);
    if (res < 0) {
        return res;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * heapTable->total;
    memset(heapTable->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);

    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t value) {
    if (value % ZEOS_HEAP_BLOCK_SIZE == 0) return value;
    value = value - (value % ZEOS_HEAP_BLOCK_SIZE) + ZEOS_HEAP_BLOCK_SIZE;
    return value;
}

static int get_heap_entry_type(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0x0f;
}

int heap_get_start_block(struct heap *heap, uint32_t total_blocks) {
    struct heap_table *table = heap->heapTable;
    int current_block = 0;
    int block_start = -1;
    for (size_t i = 0; i < table->total; ++i) {
        if (get_heap_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE) {
            current_block = 0;
            block_start = -1;
            continue;
        }
        if (block_start == -1) {
            block_start = i;
        }
        ++current_block;
        if (current_block == total_blocks) {
            break;
        }
    }
    if (block_start == -1) {
        return -ENOMEM;
    }
    return block_start;
}

void *heap_block_to_address(struct heap *heap, uint32_t start_block) {
    return heap->start_address + (start_block * ZEOS_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_token(struct heap *heap, uint32_t start_block, uint32_t total_blocks) {
    uint32_t end_block = start_block + total_blocks - 1;
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1) {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }
    for (size_t i = start_block; i <= end_block; ++i) {
        heap->heapTable->entries[i] = entry;
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        if (i != end_block - 1) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks) {
    void *address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0)
        return 0;
    address = heap_block_to_address(heap, start_block);
    heap_mark_blocks_token(heap, start_block, total_blocks);
    return address;
}

void *heap_malloc(struct heap *heap, size_t size) {
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / ZEOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

int heap_address_to_block(struct heap *heap, void *ptr) {
    return ((int) (ptr - heap->start_address)) / ZEOS_HEAP_BLOCK_SIZE;
}

void heap_mark_blocks_free(struct heap *heap, int starting_block) {
    struct heap_table *table = heap->heapTable;
    for (int i = starting_block; i < table->total; ++i) {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT)) {
            break;
        }
    }
}

void heap_free(struct heap *heap, void *ptr) {
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}