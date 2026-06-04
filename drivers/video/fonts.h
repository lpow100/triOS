#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

#define FONT_HEIGHT 16
#define FONT_WIDTH 8

extern unsigned char font8x16[128][16];

void drawChar(int x, int y, char chr, uint32_t color);
void drawString(int x, int y, const char* text, uint32_t color);

#endif