#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_

#include "console.h"

#define assert(x, info)               \
  do {                                \
    if (!(x)) {                       \
      panic(info);                    \
    }                                 \
  } while (0);

#define static_assert(x)              \
  switch(x) { case 0: case (x): ;}

// Initialize the debug info.
void init_debug();

// Print the msg + the stack trace.
void panic(const char *msg);

// print the current registers info.
void print_cur_status();

// Print function for kernel.
void printk(const char *format, ...);

void printk_color(real_color_t back, real_color_t fore, const char* format,
                  ...);

#endif  // INCLUDE_DEBUG_H_
