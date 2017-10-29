#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_

#include "console.h"

// Print function for kernel.
void printk(const char *format, ...);

void printk_color(real_color_t back, real_color_t fore, const char* format,
                  ...);

#endif  // INCLUDE_DEBUG_H_
