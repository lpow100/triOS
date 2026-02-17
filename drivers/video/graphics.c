#include "graphics.h"

uint32_t *fb;
uint32_t screenWidth;
uint32_t screenHeight;
uint32_t pitch_pixels;
uint8_t bpp;

int initGraphics(struct multiboot_info* mbi) {
    if (!(mbi->flags & (1 << 12))) return -1; //Isn't using graphics
    fb = (uint32_t*) (uintptr_t) mbi->framebuffer_addr;
    screenWidth = mbi->framebuffer_width;
    screenHeight = mbi->framebuffer_height;
    pitch_pixels = mbi->framebuffer_pitch / 4; // pitch is bytes per row; divide by 4 for pixels
    bpp = mbi->framebuffer_bpp;
}

void flip() {
    
}

void fill(uint32_t color){
    for (int i = 0; i < screenWidth + screenHeight * pitch_pixels; i++) fb[i] = color;
}

void drawPixel(int x, int y, uint32_t color){
    fb[y * pitch_pixels + x] = color;
}

//void drawRect(int x, int y, uin)