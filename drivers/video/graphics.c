#include "graphics.h"

uint32_t* fb;

int initGraphics(uint64_t framebuffer_address) {
    fb = (uint32_t*)(uintptr_t)framebuffer_address;
}

void flip() {
    
}

void fill(uint32_t color){
    for (uint32_t y = 0; y < screen_height; y++) {
        for (uint32_t x = 0; x < screen_width; x++) {
            drawPixel(x,y,color);
        }
    }
}

void drawPixel(int x, int y, uint32_t color) {
    // Prevent drawing outside the valid screen boundaries
    if (x < 0 || x >= (int)screen_width || y < 0 || y >= (int)screen_height) {
        return;
    }
    
    uint32_t index = (y * (pitch / 4)) + x;
    fb[index] = color;
}

void drawRect(int x, int y, int width, int height, uint32_t color) {
    for (uint32_t loopy = y; loopy < y + height; loopy++) {
        for (uint32_t loopx = x; loopx < x + width; loopx++) {
            drawPixel(loopx,loopy,color);
        }
    }
}