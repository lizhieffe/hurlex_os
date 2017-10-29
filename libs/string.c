#include "string.h"

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len) {
  int i;
  while (i++ < len) {
    *dest++ = *src++;
  }
}

void memset(void *dest, uint8_t val, uint32_t len) {
  uint8_t *dst = (uint8_t *)dest;
  while (len-- != 0) {
    *dst++ = val;
  }
}

void bzero(void *dest, uint32_t len) {
  memset(dest, 0, len);
}

int strcmp(const char *str1, const char *str2) {
  while (1) {
    uint8_t int1 = (uint8_t)(*str1++);
    uint8_t int2 = (uint8_t)(*str2++);
    if (int1 == 0 && int2 == 0) {
      return 0;
    } else if (int1 < int2) {
      return -1;
    } else if (int1 > int2) {
      return 1;
    }
  }
}

char *strcpy(char *dest, const char *src) {
  char *ret = dest;
  while (1) {
    *dest++ = *src;
    if (*src == 0) {
      return ret;
    }
    src++;
  }
}

char *strcat(char *dest, const char *src) {
  char *ret = dest;
  // find the starting location to paste the |src|.
  while (*dest) {
    dest++;
  }
  strcpy(dest, src);
  return ret;
}

int strlen(const char *src) {
  uint32_t len = 0;
  while (*src++) {
    len++;
  }
  return len;
}
