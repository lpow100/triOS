#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "grub.h"

#include <stdint.h>

extern uint32_t *frontBuffer;
extern uint32_t *backBuffer;
extern uint32_t screenWidth;
extern uint32_t screenHeight;
extern uint32_t pitch_pixels;
extern uint8_t bpp;

int initGraphics(struct multiboot_info* mbi);

void flip();

void fill(uint32_t color);

void drawPixel(int x, int y, uint32_t color);
void drawRect();

#endif