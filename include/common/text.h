#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <io.h>

extern size_t terminalRow;
extern size_t terminalColumn;
extern uint16_t* terminalBuffer;
extern bool isInitialized;

extern int VGA_HEIGHT;
extern int VGA_WIDTH;

size_t strlen(const char* str);

int strcmp(const char* str1, const char* str2);
int memcmp(const void* str1, const void* str2, size_t n);

void intAsString(int number, char* buffer);

void u64AsString(uint64_t number, char* buffer);

char* hexAsString(int number);

int stringAsInt(char* buffer);
uint64_t stringAsU64(const char* buffer);
uint64_t hexStringAsU64(const char* buffer);

void printU64(int number);

#endif