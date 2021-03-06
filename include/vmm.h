#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

#include "types.h"

// Where the virtual memory starts. (3GB)
#define PAGE_OFFSET 0xC0000000

/**
 * P-- 位 0 是存在 (Present) 标志，用于指明表项对地址转换是否有效。
 * P = 1 表示有效； P = 0 表示无效。
 * 在页转换过程中，如果说涉及的页目录或页表的表项无效，则会导致一个异常。
 * 如果 P = 0 ，那么除表示表项无效外，其余位可供程序自由使用。
 * 例如，操作系统可以使用这些位来保存已存储在磁盘上的页面的序号。
 */
#define PAGE_PRESENT 0x1

/** 
 * R/W -- 位 1 是读 / 写 (Read/Write) 标志。如果等于 1 ，表示页面可以被读、写或执行。
 * 如果为 0 ，表示页面只读或可执行。
 * 当处理器运行在超级用户特权级 （级别 0,1 或 2） 时，则 R/W 位不起作用。
 * 页目录项中的 R/W 位对其所映射的所有页面起作用。
 */
#define PAGE_WRITE 0x2

/**
 * U/S -- 位 2 是用户 / 超级用户 (User/Supervisor) 标志。
 * 如果为 1 ，那么运行在任何特权级上的程序都可以访问该页面。
 * 如果为 0 ，那么页面只能被运行在超级用户特权级 (0,1 或 2) 上的程序访问。
 * 页目录项中的 U/S 位对其所映射的所有页面起作用。
 */
#define PAGE_USER   0x4

#define PAGE_SIZE 4096  // page size (2^12 bytes)

// 页掩码，用于 4KB 对齐
// The least significant 12 bits in pgd_t are control bits. The most significant
// 20 bits + 000000000000 are the physical address. Here we filter out the
// control bits.
#define PAGE_MASK      0xFFFFF000

// Given a virtual address, return the page directory index.
// 0x3FF = 0011 1111 1111
// This implies that a pgd entry manages 4MB virtual memory.
#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)

// Similar as above, but instead return the page table idnex.
#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)

// Page directory entry.
typedef uint32_t pgd_t;

// Page table entry.
typedef uint32_t pte_t;

#define PGD_SIZE (PAGE_SIZE/sizeof(pte_t))

#define PTE_SIZE (PAGE_SIZE/sizeof(uint32_t))

// Count of page tables needed for mapping 512MB memory. Or to say we need 128
// page directory entries (page directory entry is one to one mapping to page
// table).
// How to get the number:
// One page table is fit into 1 page of memory (4096 Bytes). One page table
// entry is 4B, and thus one page table has 4096/4=1024 entries. One entry
// maps to one page (4096 Bytes), so one page table manages 1024*4096B=4MB. For
// 512MB, we need 512/4=128 page tables.
#define PTE_COUNT 128

// Page directory after paging enabled.
extern pgd_t pgd_kern[PGD_SIZE];

// Init virtual memory.
void init_vmm();

// Switch the page directly. |pd| should be physical address of the new page
// directory.
void switch_pgd(uint32_t pd);

// Map the pa to va. |flags| specifies the page setting.
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

// Unmap the va.
void unmap(pgd_t *pgd_now, uint32_t va);

// 如果虚拟地址 va 映射到物理地址则返回 1
// 同时如果 pa 不是空指针则把物理地址写入 pa 参数
// The returned pa is aligned with 4096 Bytes. (TODO: should we align? why?)
uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

// Interrupt handler for page fault (id 14).
void page_fault_handler();

#endif  // INCLUDE_VMM_H_
