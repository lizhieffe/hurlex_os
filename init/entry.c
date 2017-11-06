#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "timer.h"
#include "types.h"

int kern_entry() {
  init_debug();
  init_gdt();
  init_idt();

  console_clear();

  // char greeting[] = "Hello, world!\n\t- by lizhi\n";
  // console_write_color(greeting, rc_black, rc_green);

  printk_color(rc_black, rc_green, "Hello, world!\n\t- by %s\n", "lizhi");

  init_timer(200);

  // Enable the interrupt.
  // asm volatile ("sti");

  printk("kernel in memory start: 0x%08X\n", kern_start);
  printk("kernel in memory end:   0x%08X\n", kern_end);
  printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start + 1023) / 1024);

  show_memory_map();

  return 0;
}
