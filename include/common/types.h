#ifndef TYPES_H
#define TYPES_H

typedef struct {
    float x;
    float y
} __attribute__ ((packed)) Vector2;

typedef struct {
    int x;
    int y
} __attribute__ ((packed)) Vector2i;

typedef struct {
    unsigned int x;
    unsigned int y;
} __attribute__ ((packed)) uVector2i;

#endif
