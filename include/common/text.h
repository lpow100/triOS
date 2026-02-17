#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

extern size_t terminalRow;
extern size_t terminalColumn;
extern uint16_t* terminalBuffer;
extern bool isInitialized;

extern int VGA_HEIGHT;
extern int VGA_WIDTH;

size_t strlen(const char* str);

bool strcmp(const char* str1, const char* str2);

char *intAsString(int number);

char* hexAsString(int number);

int stringAsInt(char* buffer);

#endif