#include "console.h"
#include "debug.h"
#include "types.h"

int kern_entry() {
  uint8_t *input = (uint8_t *)0xB8000;
  uint8_t color = (0 << 4) | (15 & 0x0F);

  init_debug();
  console_clear();

  // char greeting[] = "Hello, world!\n\t- by lizhi\n";
  // console_write_color(greeting, rc_black, rc_green);

  printk_color(rc_black, rc_green, "Hello, world!\n\t- by %s\n", "lizhi");

  panic("test by lizhi");

  return 0;
}
