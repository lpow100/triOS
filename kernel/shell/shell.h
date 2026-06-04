#ifndef BOARD_H
#define BOARD_H

#include "text.h"
#include <video/colors.h>
#include <types.h>

#define TERMINAL_BUFFER_SIZE 65535

enum TerminalState {
    TERMINAL_TYPING, TERMINAL_WAITING
};

typedef struct Terminal{
    char buffer[TERMINAL_BUFFER_SIZE];
    uint32_t height; uint32_t width;
    uint16_t inputHead; uint16_t inputTail;
    uint32_t scroll;
    enum TerminalState state;
} Terminal;

extern Terminal globalTerminal;

void InitializeTerminal(u32Vector2 screenSize);

void AddTerminalInput(char pressed);
void AddTerminalText(char *string);

void DrawTerminal();
 
#endif