#include "shell.h"
#include <video/fonts.h>
#include <heap.h>

Terminal globalTerminal;

void InitializeTerminal(u32Vector2 screenSize) {
    globalTerminal.height = screenSize.x; globalTerminal.width = screenSize.y;
    globalTerminal.inputHead = 0; globalTerminal.inputTail = 0;
    globalTerminal.scroll = 0;
    globalTerminal.state = TERMINAL_BUFFER_SIZE;
    AddTerminalText("TRI OS - Trinary 2026\ntriOS: ~ $ ");
}

void ProcessCommand() {
    char *command = malloc(globalTerminal.inputTail - globalTerminal.inputHead);
    for (int i = 0; i < globalTerminal.inputTail - globalTerminal.inputHead - 1; i++) {
        command[i] = globalTerminal.buffer[i + globalTerminal.inputHead];
    }
    command[globalTerminal.inputTail - globalTerminal.inputHead - 1] = '\0';
    kprintf(command);
    globalTerminal.inputHead = globalTerminal.inputTail;
    if (strcmp(command, "help") == 0) { AddTerminalText("Commands are:\nhelp: prints this message\n"); }
    else { AddTerminalText("I don't know that command!\n"); }
    AddTerminalText("triOS: ~ $ ");
    free(command);
}

void AddTerminalInput(char pressed) {
    globalTerminal.buffer[globalTerminal.inputTail] = pressed;
    globalTerminal.inputTail ++;
    if (pressed == '\n') { 
        ProcessCommand();
    }
}

void AddTerminalText(char *string) {
    kprintf(string);
    //if (globalTerminal.inputHead != globalTerminal.inputTail) return; // Don't add text while someone is typing
    for (int i = 0; i < strlen(string); i++) {
        globalTerminal.buffer[globalTerminal.inputHead + i] = string[i];
    }
    globalTerminal.inputHead += strlen(string);
    globalTerminal.inputTail += strlen(string);
}

void DrawTerminal() {
    u32Vector2 pos = { 0 };
    for (int i = 0; i < globalTerminal.inputTail; i++) {
        char chr = globalTerminal.buffer[i];
        if (chr == '\n') {
            pos.x = 0;
            pos.y ++;
        } else {
            drawChar(pos.x * FONT_WIDTH, pos.y * FONT_HEIGHT, chr, WHITE);
            pos.x ++;
        }
    }
}

/*
        if (__counter % period < period / 2) { drawRect((idx) * FONT_WIDTH + 1, (yidx+1) * FONT_HEIGHT - 3, FONT_WIDTH - 2, 3, WHITE); }
        else if (__counter % period > period / 2) { drawRect((idx) * FONT_WIDTH + 1, (yidx+1) * FONT_HEIGHT - 3, FONT_WIDTH - 2, 3 , BLACK); }
        char ch = getKeyChar(read_next_key());

        if (ch == 0) {
            continue;
        }

        if (ch == '\n') { 
            drawRect((idx) * FONT_WIDTH, (yidx) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, BLACK);
            drawRect((idx+1) * FONT_WIDTH, (yidx) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, BLACK);
            idx = 0; yidx ++; 
        } else if (ch == '\b') { 
            if (idx != 0) { idx--; }
            drawRect((idx) * FONT_WIDTH, (yidx) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, BLACK);
            drawRect((idx+1) * FONT_WIDTH, (yidx) * FONT_HEIGHT, FONT_WIDTH, FONT_HEIGHT, BLACK);
        } else { 
            drawRect((idx) * FONT_WIDTH + 1, (yidx+1) * FONT_HEIGHT - 3, FONT_WIDTH - 2, 3 , BLACK);
            drawChar(idx * FONT_WIDTH, yidx * FONT_HEIGHT, ch, WHITE); 
            idx++; 
        }*/