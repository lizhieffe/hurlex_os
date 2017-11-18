#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include "types.h"

// Kernel stack starts at 3.0GB (like linux), heap starts from 3.5GB.
#define HEAP_START 0xE0000000

// Header for a memory chunk. It is placed at the beginning of the chunk.
typedef
struct header {
  // chunk link list.
  struct header *prev;
  struct header *next;
  uint32_t allocated : 1;
  uint32_t length : 31;
} header_t;

void init_heap();

void *kmalloc(uint32_t len);

void kfree(void *p);

// test the alloc and free of heap.
void test_heap();

#endif  // INCLUDE_HEAP_H_
