#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef struct {
    float x;
    float y
} __attribute__ ((packed)) Vector2;

typedef struct {
    int x;
    int y
} __attribute__ ((packed)) intVector2;

typedef struct {
    uint32_t x;
    uint32_t y;
} __attribute__ ((packed)) u32Vector2;


typedef struct {
    uint64_t x;
    uint64_t y;
} __attribute__ ((packed)) u64Vector2;

#endif
