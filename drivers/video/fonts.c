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


void drawString(int x, int y, const char* text, uint32_t color){
    for (int i = 0; text[i] != '\0'; i++) {
        drawChar(x + i * FONT_WIDTH, y, text[i], color);
    } 
}