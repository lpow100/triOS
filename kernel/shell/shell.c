#include "shell.h"
#include <video/fonts.h>

bool onGlobalTerminal = true;
Terminal globalTerminal;

void terminalInitialize(Terminal *term, uint32_t width, uint32_t height){
    term->height = height; term->width = width;
    term->inputHead = 0; term->inputTail = 0;
    term->scroll = 0;
    term -> state = INPUTTING;
}

void terminalDraw(Terminal *term, int xPos, int yPos){
    int x = 0;
    int y = -term->scroll;
    for (int i = 0; term -> buffer[i] != '\0'; i++){
        if (x >= (term -> width / FONT_WIDTH)) {
            x = 0; y ++;
        }
        if (term -> buffer[i] == '\n'){
            x = 0; y ++;
        } else if (term -> buffer[i] == '\t') {
            x += 4;
        } else {
            drawChar(xPos + (x*8), yPos + (y*16), term -> buffer[i], WHITE);
            x++;
        }
    }
}

void terminalWriteChar(Terminal *term, char chr) {
    // subtract one becuase we leave on space for the \0
    if (term -> inputTail >= TERMINAL_BUFFER_SIZE - 1) term -> inputTail = TERMINAL_BUFFER_SIZE - 2;
    term -> buffer[term -> inputTail] = chr;
    term -> buffer[term -> inputTail + 1] = '\0';
    term -> inputTail += 1;
}
void terminalWrite(Terminal *term, char *text){
    for (int i = 0; text[i] != '\0'; i++) {
        terminalWriteChar(term,text[i]);
    }
}

void analayzeInput(Terminal *term) {
    char words[128][256];
    int idx = 0;
    int wordIdx = 0;
    words[0][0] = '\0'; 
    for (int i = 0; i < term -> inputHead - term -> inputTail; i++){
        if (term -> buffer[term -> inputHead + i] == '\0') break;
        if (term -> buffer[term -> inputHead + i] == ' '){
            if (strlen(words[idx]) == 0){
                continue;
            }
            idx ++; wordIdx = 0;
            continue;
        }
        words[idx][wordIdx] = term -> buffer[term -> inputHead + i];
        words[idx][wordIdx + 1] = '\0';
        wordIdx ++;
    } 
    int wordCount = idx; // rename for clarity
    if (term -> state == INPUTTING) {
        if (strcmp(words[0], "help")) terminalWrite(term, "help coming");
        if (strcmp(words[0], "echo")) {
            for (int i = 1; i < wordCount + 1; i++) {
                terminalWrite(term, words[i]);
                terminalWrite(term, " ");
            }
            terminalWrite(term, "\n");
        }
    }
    terminalWrite(term, "user: ~ $ ");
    term -> state = INPUTTING;
    term -> inputHead = term -> inputTail;
}

char *helpCommand = "triOS shell commands:\nhelp - prints this command\necho - repeats what you say\n\nmore comamands coming soon...\n";

/*void initShell() {
    terminalWrite("triOS TTY - trinary 2025\n");
    terminalWrite("user: ~ $ ");
    term -> state = INPUTTING;
}*/