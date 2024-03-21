#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdalign.h>
#include "config.h"
#include <assert.h>

#define NUM_ENTRIES ((1 << (POBITS - 3)) - 1)

size_t ptbr = 0;

void *allocate_new_page() {
    void *page;
    if (posix_memalign(&page, 1 << POBITS, 1 << POBITS) != 0) {
        fprintf(stderr, "Failed to allocate a new page\n");
        exit(EXIT_FAILURE);
    }
    memset(page, 0, 1 << POBITS);
    return page;
}

void ensure_page_table() {
    if (!ptbr) {
        ptbr = (size_t)allocate_new_page();
    }
}

void page_allocate(size_t va) {
    ensure_page_table();

    size_t *current_pt = (size_t *)ptbr;
    for (int level = 0; level < LEVELS; ++level) {
        size_t index_shift = POBITS + (LEVELS - level - 1) * (POBITS - 3);
        size_t index = (va >> index_shift) & NUM_ENTRIES;
        size_t *curr_index_ptr = &current_pt[index];

        if (!(*curr_index_ptr & 1)) {
            void *new_page = allocate_new_page();
            *curr_index_ptr = ((size_t)new_page | 1);
        }

        if (level < LEVELS - 1) {
            current_pt = (size_t *)(*curr_index_ptr & ~((size_t)1));
        }
    }
}

size_t translate(size_t va) {
    if (!ptbr) return (size_t)-1;

    size_t *current_pt = (size_t *)ptbr;
    for (int level = 0; level < LEVELS; ++level) {
        size_t index_bits = POBITS + (LEVELS - level - 1) * (POBITS - 3);
        size_t index = (va >> index_bits) & NUM_ENTRIES;
        size_t curr_index = current_pt[index];

        if (!(curr_index & 1)) {
            return (size_t)-1;
        }

        if (level < LEVELS - 1) {
            current_pt = (size_t *)(curr_index & ~((size_t)1));
        } else {
            size_t page_offset = va & ((1 << POBITS) - 1);
            return ((curr_index & ~((size_t)1)) | page_offset);
        }
    }

    return (size_t)-1;
}

int main() {
    // 0 pages have been allocated
    assert(ptbr == 0);

    page_allocate(0x456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x456789abcd00);
    // no new pages allocated (still 5)

    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
}