#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "heap.h"
#include "idt.h"
#include "mm_util.h"
#include "multiboot.h"
#include "pmm.h"
#include "timer.h"
#include "types.h"
#include "vmm.h"

// The initialization process after paging enabled.
void kern_init();

uint32_t kern_stack_top;

// global multibool pointer after paging enabled
multiboot_t *glb_mboot_ptr;

// kernel stack after paging enabled.
char kern_stack[STACK_SIZE];

int thread_test_flag = 0;
static int thread_test(void *arg);

// The page directory and page table used used by kernel before paging enabled.
// These address must be aligned (why?). 0-640KB are (certainly) free.
__attribute__((section(".init.data"))) pgd_t *pgd_tmp = (pgd_t *)0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low = (pgd_t *)0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *)0x3000;

// The initialization process before mmu enabled.
__attribute__((section(".init.text"))) void kern_entry() {
  pgd_tmp[0] = (uint32_t)pte_low | PAGE_PRESENT | PAGE_WRITE;
  pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (uint32_t)pte_high | PAGE_PRESENT | PAGE_WRITE;

  int i;
  // 映射内核虚拟地址 4MB 到物理地址的前 4MB (TODO: is this 4MB or 1MB ??)
  for (i = 0; i < 1024; i++) {
    // 2^12 = 4KB. This implies that one pte entry manages 4KB of virtual memory.
    pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
  }
  for (i = 0; i < 1024; i++) {
    // 映射 0x00000000-0x00400000 的物理地址到虚拟地址 0xC0000000-0xC0400000
    // TODO: is this 4MB interval or 1MB?
    pte_high[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
  }

  // set temp pgd
  asm volatile ("mov %0, %%cr3" : : "r" (pgd_tmp));

  uint32_t cr0;

  // Enable paging by setting the paging bit in register cr0 to 1.
  // 设置临时页表
  asm volatile ("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= 0x80000000;
  asm volatile ("mov %0, %%cr0" : : "r" (cr0));
  
  // 切换内核栈
  kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
  asm volatile ("mov %0, %%esp\n\t"
  		"xor %%ebp, %%ebp" : : "r" (kern_stack_top));

  // Update global multiboot_t pointer.
  glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;

  kern_init();
}

void kern_init() {
  init_debug();
  init_gdt();
  init_idt();

  console_clear();
  printk_color(rc_black, rc_green, "Hello, world!\n\t- by %s\n", "lizhi");

  init_timer(200);

  printk("kernel in memory start: 0x%08X\n", kern_start);
  printk("kernel in memory end:   0x%08X\n", kern_end);
  printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start + 1023) / 1024);

  show_memory_map();
  init_pmm();
  init_vmm();

  printk_color(rc_black, rc_red,
      "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);

  uint32_t allc_addr = NULL;
  // printk_color(rc_black, rc_light_brown, "Test Physical Memory Alloc :\n");
  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);
  // allc_addr = pmm_alloc_page();
  // printk_color(rc_black, rc_light_brown, "Alloc Physical Addr: 0x%08X\n", allc_addr);

  printk_color(rc_black, rc_green, "0x%08X->0x%08x\n", 0xC0000000, (uint32_t)get_phyaddr(0xC0000000));
  printk_color(rc_black, rc_green, "0x%08X->0x%08x\n", 0xC0010020, (uint32_t)get_phyaddr(0xC0010020));

  init_heap();

  printk_color(rc_black, rc_red, "\nThe Count of Physical Memory Page is: %u\n\n", phy_page_count);
  test_heap();

  init_sched();
  kernel_thread(thread_test, NULL);

  // Enable the interrupt.
  asm volatile ("sti");

  while (1) {
    if (thread_test_flag == 0) {
      printk_color(rc_black, rc_red, "A");
      thread_test_flag = 1;
    }
  }

  while (1) {
    asm volatile ("hlt");
  }
}

int thread_test(void *arg) {
  while (1) {
    if (thread_test_flag == 1) {
      printk_color(rc_black, rc_green, "B");
      thread_test_flag = 0;
    }
  }
  return 0;
}
