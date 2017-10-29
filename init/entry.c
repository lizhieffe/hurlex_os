#include "console.h"
#include "types.h"

int kern_entry() {
  uint8_t *input = (uint8_t *)0xB8000;
  uint8_t color = (0 << 4) | (15 & 0x0F);

  char greeting[] = "Hello, world!    - by lizhi";
  console_write(greeting);

  return 0;
}
