#include "gdt.h"

#include "string.h"

#define GDT_LENGTH 5

gdt_entry_t gdt_entries[GDT_LENGTH];

// GDTR.
gdt_ptr_t gdt_ptr;

// 全局描述符表构造函数，根据下标构造
// 参数分别是 数组下标、基地址、限长、访问标志，其它访问标志
static void gdt_set_gate(int32_t num, uint32_t base,
                         uint32_t limit, uint8_t access, uint8_t gran);

// declear the kernel stack.
extern uint32_t stack;

void init_gdt() {
  gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_LENGTH - 1;
  gdt_ptr.base = (uint32_t)&gdt_entries;

  // 采用 Intel 平坦模型
  // The flat model maps all the segments to the same address space, to mimic
  // the situation that there is no segmentation. This is a regular approach to
  // disable the segmentation, because nowadays people tends to enable the
  // paging but disable the segmentation.
  gdt_set_gate(0, 0, 0, 0, 0);   // 按照 Intel 文档要求，第一个描述符必须全 0
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);     // kernel code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);     // kernel data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);     // user code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);     // user data segment

  // 加载全局描述符表地址到 GPTR 寄存器
  gdt_flush((uint32_t)&gdt_ptr);
}

void gdt_set_gate(int32_t num, uint32_t base,
                  uint32_t limit, uint8_t access, uint8_t gran) {
  gdt_entries[num].limit_low = (base & 0xFFFF);
  gdt_entries[num].limit_middle = (base << 16) & (0xFF);
  gdt_entries[num].limit_high = (base << 24) & (0xFF);

  gdt_entries[num].limit_low = (limit & 0xFFFF);
  gdt_entries[num].granularity = (limit >> 16) & 0x0F;

  gdt_entries[num].granularity |= gran & 0xF0;
  gdt_entries[num].access = access;
}
