#include "fonts.h"
#include "graphics.h"

void drawChar(int x, int y, char chr, uint32_t color) {
    for (int lineIdx = 0; lineIdx < 16; lineIdx++) {
        unsigned char line = font8x16[chr][lineIdx];
        for (int i = 0; i < 8; i++) {
            unsigned char mask = 0x80 >> i;
            if (line & mask) drawPixel(x + i, y + lineIdx, color);
        }
    }
}