#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_

#include "types.h"

// entry type for Global Descriptor Table (GDT). Or called segment descriptor.
typedef
struct gdt_entry_t {
  uint16_t limit_low;   // limit for the maximum addressable unit. Start from 0.
  uint16_t base_low;    // base 0:15. base is the linear address where the segment begins.
  uint8_t base_middle;  // base 16:31
  uint8_t access;       // 段存在位、描述符特权级、描述符类型、描述符子类别
  uint8_t granularity;  // 其他标志、段界限 19 ～ 16
  uint8_t base_high;    // base 32:48
} __attribute__((packed)) gdt_entry_t;

// GDTR. Pointer to the Global Descriptor Table.
typedef
struct gdt_ptr_t {
  uint16_t limit;       // or called size. limit for the maximum addressable unit. Start from 0.
  uint32_t base;        // address of the table.
} __attribute__((packed)) gdt_ptr_t;

void init_gdt();

// Implemented by assembly.
extern void gdt_flush(uint32_t);

#endif  // INCLUDE_GDT_H_
