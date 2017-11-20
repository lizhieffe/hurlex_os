#include "pmm.h"

#include "debug.h"
#include "multiboot.h"

// Kernel physical memory stack.
static uint32_t pmm_stack[PAGE_MAX_SIZE + 1];

// Pointer to the kernel physical memory stack top.
static uint32_t pmm_stack_top;

// Count of physical pages, including both the free and non-free ones.
uint32_t phy_page_count;

void show_memory_map() {
  uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
  uint32_t mmap_length = glb_mboot_ptr->mmap_length;

  printk("Available physical memory blocks:\n");

  mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
  for (mmap = (mmap_entry_t *)mmap_addr;
       (uint32_t)mmap < mmap_addr + mmap_length;
       mmap++) {
    printk("base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
           (uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
           (uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
           (uint32_t)mmap->type);
  }
}

void init_pmm() {
  printk_color(rc_black, rc_red, "PAGE_MAX_SIZE = %d\n", PAGE_MAX_SIZE);

  mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
  mmap_entry_t *mmap_end_addr = mmap_start_addr + glb_mboot_ptr->mmap_length;

  mmap_entry_t *map_entry;
  for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {
    // type == 1 means the RAM is usable.
    // 0x100000 = 1MB. Here we only use memory above 1MB physical memory because
    // there are many devices' port mapped to physical address below 1MB.
    // 
    // TODO: here we hardcoded 1MB as base_addr_low because (for some reason)
    // the available physical memory is always from 0x0-0x00008FC00 and
    // 0x000100000-UPPER_VALUE_DEPENDS_ON_HARDWARD.
    if (map_entry->type == 1 && map_entry->base_addr_low == 0x100000) {
      // TODO: are the line below correct?
      uint32_t page_addr = map_entry->base_addr_low +
                           (uint32_t)(kern_end - kern_start);
      uint32_t length = map_entry->base_addr_low + map_entry->length_low;

      // TODO: remove this
      while (page_addr < length && page_addr <= PMM_MAX_SIZE) {
        pmm_free_page(page_addr);
        page_addr += PMM_PAGE_SIZE;
        phy_page_count++;
      }
    }
  }
}

uint32_t pmm_alloc_page() {
  assert(pmm_stack_top != 0, "out of memory");
  uint32_t page = pmm_stack[pmm_stack_top--];
  return page;
}

void pmm_free_page(uint32_t p) {
  assert(pmm_stack_top != PAGE_MAX_SIZE, "out of pmm_stack stack");
  pmm_stack[++pmm_stack_top] = p;
}
