#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
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
  asm volatile ("sti");

  asm volatile("int $0x3");
  asm volatile("int $0x4");

  return 0;
}
