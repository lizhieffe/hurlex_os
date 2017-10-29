#include "console.h"
#include "types.h"

int kern_entry() {
  uint8_t *input = (uint8_t *)0xB8000;
  uint8_t color = (0 << 4) | (15 & 0x0F);

  console_clear();

  char greeting[] = "Hello, world!\n\t- by lizhi";
  console_write_color(greeting, rc_black, rc_green);

  return 0;
}
