// Page memory management.

#ifndef INCLUDE_PMM_H_
#define INCLUDE_PMM_H_

#include "multiboot.h"

// 线程栈的大小
#define STACK_SIZE 8192

// Max physical memory supported 512 MB.
#define PMM_MAX_SIZE 0x20000000

// Divide the physical memory to page with size 4096 KB. This size is the same
// size as the size used for virtual memory in the paging mechanism.
#define PMM_PAGE_SIZE 0x1000

// Number of pages supported.
#define PAGE_MAX_SIZE (PMM_MAX_SIZE/PMM_PAGE_SIZE)

// 页掩码 按照 4096 对齐地址
#define PHY_PAGE_MASK 0xFFFFF000

// Start and end of kernel in memory. Defined in kernel.ld.
extern uint8_t kern_start[];
extern uint8_t kern_end[];

// Count of the physical pages that is currently allocated.
extern uint32_t phy_page_count;

// Print the physical memory layout given by BIOS.
void show_memory_map();

void init_pmm();

// Allocate memory from the available memory in kernel stack and returns its
// physical address.
uint32_t pmm_alloc_page();

// Free the given physical memory and return it to the kernel stack as available
// memory.
void pmm_free_page(uint32_t p);

#endif  // INCLUDE_PMM_H_
