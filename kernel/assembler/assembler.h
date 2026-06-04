#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>
#include <stdbool.h>
#include <lib.h>
#include <text.h>
#include <memory.h>
#include <system/setjmp.h>

typedef struct {
    uint8_t* bytes;
    size_t size;
} AssembledCode;

AssembledCode assemble(const char* assembly, int size);

#endif