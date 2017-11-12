#include "console.h"

#include "common.h"
#include "vmm.h"

uint8_t kConsoleWidth = 80;
uint8_t kConsoleHeight = 25;

// Address of the start of the VGA memory.
static uint16_t *video_memory = (uint16_t *)(0xB8000 + PAGE_OFFSET);

// Cursor location.
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void move_cursor() {
  // Screen is 80 pixel in width.
  uint16_t cursor_location = cursor_y * kConsoleWidth + cursor_x;

  // 在这里用到的两个内部寄存器的编号为14与15，分别表示光标位置的高8位与低8位。
  outb(0x3D4, 14);  // inform VGA that the upper part of the cursor location is going to be set.
  outb(0x3D5, cursor_location >> 8);  // send the upper part.
  outb(0x3D4, 15);  // inform VGA that the lower part of the cursor location is going to be set.
  outb(0x3D5, cursor_location);  // send the lower part.
}

// Scroll one line upwards if the cursor in y direction is below the bottom of
// the screen.
static void scroll() {
  // attribute_byte 被构造出一个黑底白字的描述格式
  uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
  uint16_t blank = 0x20 | (attribute_byte << 8);  // space 是 0x20

  // cursor_y 到 25 的时候，就该换行了
  if (cursor_y >= 25) {
    // 将所有行的显示数据复制到上一行，第一行永远消失了...
    int i;
    
    for (i = 0 * kConsoleWidth; i < 24 * kConsoleWidth; i++) {
          video_memory[i] = video_memory[i + kConsoleWidth];
    }

    // 最后的一行数据现在填充空格，不显示任何字符
    for (i = 24 * kConsoleWidth; i < 25 * kConsoleWidth; i++) {
          video_memory[i] = blank;
    }
    
    // 向上移动了一行，所以 cursor_y 现在是 24
    cursor_y = 24;
  }
}

void console_clear() {
  uint8_t attribute_byte = (0 << 4) | (15 & 0x0F);
  uint16_t blank = 0x20 | (attribute_byte << 8);

  // Use blank to cover all the screen.
  int i;
  for (i = 0; i < kConsoleWidth * kConsoleHeight; i++) {
    video_memory[i] = blank;
  }

  cursor_x = 0;
  cursor_y = 0;
  move_cursor();
}

void console_putc_color(char c, real_color_t back, real_color_t fore) {
  uint8_t back_color = (uint8_t)back;
  uint8_t fore_color = (uint8_t)fore;

  uint8_t attribute_byte = (back_color << 4) | (fore_color & 0x0F);
  uint16_t attribute = attribute_byte << 8;

  // 0x08 is backspace in ASCII.
  // 0x09 is tab in ASCII.
  if (c == 0x08 && cursor_x) {
    cursor_x--;
  } else if (c == 0x09) {
    cursor_x = (cursor_x + 8) & ~(8 - 1);
  } else if (c == '\r') {
    cursor_x = 0;
  } else if (c == '\n') {
    cursor_x = 0;
    cursor_y++;
  } else if (c >= ' ') {
    video_memory[cursor_y * kConsoleWidth + cursor_x] = c | attribute;
    cursor_x++;
  }

  if (cursor_x >= kConsoleWidth) {
    cursor_x = 0;
    cursor_y++;
  }

  scroll();

  move_cursor();
}

void console_write(char *cstr) {
  console_write_color(cstr, rc_black, rc_white);
}

void console_write_color(char *cstr, real_color_t back, real_color_t fore) {
  while (*cstr) {
    console_putc_color(*cstr++, back, fore);
  }
}

void console_write_hex(uint32_t n, real_color_t back, real_color_t fore) {
}

void console_write_dec(uint32_t n, real_color_t back, real_color_t fore) {
}
