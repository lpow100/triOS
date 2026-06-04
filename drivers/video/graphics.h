#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "grub.h"

#include <stdint.h>

extern uint32_t screen_width;
extern uint32_t screen_height;
extern uint32_t bits_per_pixel;
extern uint32_t pitch;

int initGraphics(uint64_t framebuffer_address);

void fill(uint32_t color);

void drawPixel(int x, int y, uint32_t color);
void drawRect(int x, int y, int width, int height, uint32_t color);

#endif