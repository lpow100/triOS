#ifndef LIB_H
#define LIB_H

#include <stdint.h>
#include <stddef.h>

void memcpy(void* dest, const void* src, size_t n);
static inline int abs(int x)
{
  return x >= 0 ? x : -x;
}
char *itoa(int i);
void srand();
uint32_t rand(void);

#endif