#include "lib.h"
#include "io.h"

void memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}
#define INT_DIGITS 19 /* enough for 64 bit integer */

char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1; /* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

static uint32_t __x = 123456789;
static uint32_t __y = 362436069;
static uint32_t __z = 521288629;
static uint32_t __w = 88675123;

void srand(uint32_t s)
{
  __x = s;
}

uint32_t rand(void)
{
  uint32_t t;
  t = __x ^ (__x << 11);
  __x = __y; __y = __z; __z = __w;
  return __w = __w ^ (__w >> 19) ^ (t ^ (t >> 8));
}

int oct2bin(unsigned char *str, int size) {
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}