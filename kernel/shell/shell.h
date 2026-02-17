#ifndef BOARD_H
#define BOARD_H

#include "text.h"
#include <video/colors.h>

#define TERMINAL_BUFFER_SIZE 65535

#define INPUTTING 0
#define PROCESSING 1
#define INACTIVE 2

typedef char terminalState;

typedef struct Terminal{
    char buffer[TERMINAL_BUFFER_SIZE];
    uint32_t height; uint32_t width;
    uint16_t inputHead; uint16_t inputTail;
    uint32_t scroll;
    terminalState state;
} Terminal;

extern bool onGlobalTerminal;
extern Terminal globalTerminal;

// Sets a terminal up with it's width and height
void terminalInitialize(Terminal *term, uint32_t width, uint32_t height);

// Draws a terminal to the screen at a given pos
void terminalDraw(Terminal *term, int xPos, int yPos);

// adds a charecter to the terminal buffer
void terminalWriteChar(Terminal *term, char chr);
// adds a string to the terminal buffer
void terminalWrite(Terminal *term, char *text);

void analayzeInput(Terminal *term);
 
#endif