#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

# include "types.h"

// Write a char to port.
void outb(uint16_t port, uint8_t value);

// Read a char from port.
uint8_t inb(uint16_t port);

// Read a word from port.
uint16_t intw(uint16_t port);

#endif  // INCLUDE_COMMON_H_
