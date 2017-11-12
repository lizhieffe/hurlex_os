#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

// Where the virtual memory starts.
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

#define PAGE_SIZE 4096  // page size (2^12 bytes)

// Given a virtual address, return the page directory index.
// 0x3FF = 0011 1111 1111
// This implies that a pgd entry manages 4MB virtual memory.
#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)

// Page directory entry.
typedef uint32_t pgd_t;

// Page table entry.
typedef uint32_t pte_t;

#endif  // INCLUDE_VMM_H_
