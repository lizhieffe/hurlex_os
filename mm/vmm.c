#include "vmm.h"

#include "idt.h"
#include "pmm.h"
#include "string.h"

// Page directory after paging enabled.
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));

void init_vmm() {
  uint32_t kern_pte_first_index = PGD_INDEX(PAGE_OFFSET);

  uint32_t i, j;
  for (i = kern_pte_first_index, j = 0; i < PTE_COUNT + kern_pte_first_index;
       i++, j++) {
    // MMU needs the physical address. Here PAGE_OFFSET is substracted.
    pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
  }

  uint32_t *pte = (uint32_t *)pte_kern;
  // 不映射第 0 页，便于跟踪 NULL 指针
  //
  // Because i < PTE_COUNT*PTE_SIZE, here we actually set up all the items in
  // the 2D pte_kern.
  // 
  // TODO: why the start of physical memory is from 0 (0 << 12) or 4KB
  // (1 << 12)? Many locations in the physical memory below 1MB are used by
  // device port.
  for (i = 1; i < PTE_COUNT * PTE_SIZE; i++) {
    pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
  }

  register_interrupt_handler(14, &page_fault_handler);

  switch_pgd((uint32_t)pgd_kern - PAGE_OFFSET);
}

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags) {
  uint32_t pgd_idx = PGD_INDEX(va);
  uint32_t pte_idx = PTE_INDEX(va);

  pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
  if (!pte) {
    // A page table occupies one page.
    pte = (pte_t *)pmm_alloc_page();
    pgd_now[pgd_idx] = (uint32_t)pte | PAGE_PRESENT | PAGE_WRITE;

    // Switch to kernel linear space.
    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
    // Clear.
    bzero(pte, PAGE_SIZE);
  } else {
    // Switch to kernel linear space.
    pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
  }
  pte[pte_idx] = (pa & PAGE_MASK) | flags;

  // 通知 CPU 更新页表缓存
  asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *pgd_now, uint32_t va) {
  uint32_t pgd_idx = PGD_INDEX(va);
  uint32_t pte_idx = PTE_INDEX(va);

  pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
  if (!pte) {
    return;
  }

  // Switch to kernel linear space.
  pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

  pte[pte_idx] = 0;

  // 通知 CPU 更新页表缓存
  asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa) {
  uint32_t pgd_idx = PGD_INDEX(va);
  uint32_t pte_idx = PTE_INDEX(va);

  pte_t *pte = pgd_now[pgd_idx];
  if (!pte) {
    return 0;
  }

  // Switch to kernel linear space.
  pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

  if (pte[pte_idx] == 0 || !pa) {
    return 0;
  }

  // TODO: should we count the last 12 bits of va?
  *pa = pte[pte_idx] & PAGE_MASK;
  return 1;
}

void switch_pgd(uint32_t pd) {
      asm volatile ("mov %0, %%cr3" : : "r" (pd));
}
