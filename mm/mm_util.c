#include "mm_util.h"

#include "debug.h"
#include "vmm.h"

void * get_phyaddr(void * virtualaddr) {
  uint32_t pgd_index = PGD_INDEX((uint32_t)virtualaddr);
  uint32_t pte_index = PTE_INDEX((uint32_t)virtualaddr);

  // Values in pde is physical address. But the OS already has paging turned on.
  pgd_t pde = pgd_kern[pgd_index] + PAGE_OFFSET;
  // The least significant 12 bits of page directory entry are control bits. The
  // rest are physical address. This is also why the page table has to align
  // with 4096 Bytes (2^12 for the control bits)g
  pte_t *pte_start = (pte_t *)(pde & 0xFFFFF000);
  pte_t pte = pte_start[pte_index];

  // Similar as above, the least significant 12 bits of page table entry are
  // also control bits. The part on the right of '+' is the offset (least
  // significant 12 bits) from virtualaddr.
  return (void *)((pte & ~0xFFF) + ((uint32_t)virtualaddr & 0xFFF));
}
