# README for Multi-Level Page Table Project

## Customizing `config.h`

The `config.h` file is crucial for the configuration of the multi-level page table simulation. It defines two constants:

- `LEVELS`: the number of levels in the page table. Valid values range from 1 to 6. This setting adjusts the depth of your page table structure.
- `POBITS`: the number of bits used for the page offset, which impacts the size of each page table and the size of pages themselves. Valid values range from 4 to 18.

### Guidance for Picking Values

- **LEVELS**: A higher value increases the depth of the table, which can reduce memory usage for sparse address spaces but may increase translation time. Lower values simplify the structure but may use more memory for densely populated address spaces.
- **POBITS**: This defines the size of pages (2^POBITS bytes). Larger values increase page size, reducing the number of pages and page tables needed but potentially increasing memory waste due to internal fragmentation. Smaller values decrease memory waste but increase the number of pages and page tables, which may increase memory usage for the page tables themselves.

Choose values based on the expected usage patterns and memory characteristics of the applications you are simulating.

## Known Bugs or Limitations

- No known bugs at this time.
- **Limitations**: Currently, the implementation supports only integer values for `LEVELS` and `POBITS` within specified ranges. Additionally, the implementation may not support cases where `(POBITS - 3) * (LEVELS + 1) > 60` due to address space constraints.

## Future Expansion Suggestions

- Implement support for larger virtual address spaces by increasing the maximum allowable values for `LEVELS` and `POBITS`.
- Improve error handling and reporting to provide more detailed feedback for incorrect or out-of-range addresses.
- Optimize page table traversal and memory allocation strategies for performance improvements.

## Big-O Analysis

- **Time Complexity**: The time complexity for translating a virtual address to a physical address is O(LEVELS) because each level of the page table may need to be traversed.
- **Space Complexity**: The space complexity is O(n), where n is the number of unique pages allocated, since each page requires a fixed amount of memory.

## Testing Hooks

- A counter for `posix_memalign` calls is added to monitor memory allocation frequency.
- Assertions are used extensively to ensure the correctness of page table entries and address translations.

## Code Samples

Here is how to use the library:

```c
#include "mlpt.h"
#include "config.h"

int main() {
    // Initialize ptbr to 0 as no page tables are allocated yet
    assert(ptbr == 0);

    // Allocate a new page
    page_allocate(0x12345678abcd);

    // Translate a virtual address to a physical address
    size_t pa = translate(0x12345678abcd);
    if (pa == (size_t)-1) {
        // Handle the case where the address is not mapped
    }

    // Clean up and exit
    return 0;
}
