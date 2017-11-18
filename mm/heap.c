#include "heap.h"

#include "debug.h"
#include "pmm.h"
#include "vmm.h"

static void alloc_chunk(uint32_t start, uint32_t len);

// Free the given chunk and all chunks above it (remove them from the chunk link
// list). Also unmap the corresponding pages.
static void free_chunk(header_t *chunk);

// 切分内存块
static void split_chunk(header_t *chunk, uint32_t len);

// Given an unallocated chunk, glue the unallocated chunk before and after.
static void glue_chunk(header_t *chunk);

// Top of the heap. Heap memory is allocated as multiple of PAGE_SIZE;
static uint32_t heap_max = HEAP_START;

static header_t *heap_first;

void init_heap() {
  heap_first = 0;
}

void *kmalloc(uint32_t len) {
  len += sizeof(header_t);

  header_t *cur_header = heap_first;
  header_t *prev_header = 0;

  // == 0 if this method is called the first time.
  while (cur_header) {
    if (cur_header->allocated == 0 && cur_header->length >= len) {
      split_chunk(cur_header, len);
      cur_header->allocated = 1;
      return (void *)((uint32_t)cur_header + sizeof(header_t));
    }
    prev_header = cur_header;
    cur_header = cur_header->next;
  }

  uint32_t chunk_start;
  if (prev_header) {
    chunk_start = (uint32_t)prev_header + prev_header->length;
  } else {
    chunk_start =  HEAP_START;
    heap_first = (header_t *)chunk_start;
  }

  alloc_chunk(chunk_start, len);
  cur_header = (header_t *)chunk_start;
  cur_header->prev = prev_header;
  cur_header->next = 0;
  cur_header->allocated = 1;
  cur_header->length = len;

  if (prev_header) {
    prev_header->next = cur_header;
  }

  return (void *)(chunk_start + sizeof(header_t));
}

void kfree(void *p) {
  header_t *header = (header_t *)((uint32_t)p - sizeof(header_t));
  header->allocated = 0;

  glue_chunk(header);
}

void alloc_chunk(uint32_t start, uint32_t len) {
  while (start + len > heap_max) {
    uint32_t page = pmm_alloc_page();
    map(pgd_kern, heap_max, page, PAGE_PRESENT | PAGE_WRITE);
    heap_max += PAGE_SIZE;
  }
}

void split_chunk(header_t *chunk, uint32_t len)
{
  // 切分内存块之前得保证之后的剩余内存至少容纳一个内存管理块的大小
  if (chunk->length - len > sizeof (header_t)) {
    header_t *newchunk = (header_t *)((uint32_t)chunk + len);
    newchunk->prev = chunk;
    newchunk->next = chunk->next;
    newchunk->allocated = 0;
    newchunk->length = chunk->length - len;

    chunk->next = newchunk;
    chunk->length = len;
  }
}

void free_chunk(header_t *chunk) {
  if (chunk->prev == 0) {
    heap_first = 0;
  } else {
    chunk->prev->next = 0;
  }

  // Free the memory if it is not less than one page.
  while ((heap_max - PAGE_SIZE) >= (uint32_t)chunk) {
    heap_max -= PAGE_SIZE;
    uint32_t page;
    get_mapping(pgd_kern, heap_max, &page);
    unmap(pgd_kern, heap_max);
    pmm_free_page(page);
  }
}

void glue_chunk(header_t *chunk) {
    // 如果该内存块前面有链内存块且未被使用则拼合
    if (chunk->next && chunk->next->allocated == 0) {
        chunk->length = chunk->length + chunk->next->length;
        if (chunk->next->next) {
            chunk->next->next->prev = chunk;
        }
        chunk->next = chunk->next->next;
    }

    if (chunk->prev && chunk->prev->allocated == 0) {
        chunk->prev->length = chunk->prev->length + chunk->length;
        chunk->prev->next = chunk->next;
        if (chunk->next) {
            chunk->next->prev = chunk->prev;
        }
        chunk = chunk->prev;
    }

    // 假如该内存后面没有链表内存块了直接释放掉
    if (chunk->next == 0) {
        free_chunk(chunk);
    }
}

void test_heap()
{
  printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

  void *addr1 = kmalloc(50);
  printk("kmalloc    50 byte in 0x%X\n", addr1);
  void *addr2 = kmalloc(500);
  printk("kmalloc   500 byte in 0x%X\n", addr2);
  void *addr3 = kmalloc(5000);
  printk("kmalloc  5000 byte in 0x%X\n", addr3);
  void *addr4 = kmalloc(50000);
  printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

  printk("free mem in 0x%X\n", addr1);
  kfree(addr1);
  printk("free mem in 0x%X\n", addr2);
  kfree(addr2);
  printk("free mem in 0x%X\n", addr3);
  kfree(addr3);
  printk("free mem in 0x%X\n\n", addr4);
  kfree(addr4);
}
