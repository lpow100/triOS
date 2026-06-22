#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>

void basic_print(char* string, uint32_t length) {
    asm volatile (
        "movq $1, %%rax\n\t"    // System Call ID 1 = SYS_WRITE
        "movq $1, %%rdi\n\t"    // File Descriptor 1 = STDOUT (Screen/Terminal)
        "movq %0, %%rsi\n\t"    // %0 maps to your input variable 'string' below
        "movq %1, %%rdx\n\t"    // %1 maps to your input variable 'length' below
        "syscall"               // The correct native 64-bit entry transition instruction!
        :                       // No output constraints (Leave this blank!)
        : "r"(string), "r"((uint64_t)length) // Input constraints: 'string' is %0, 'length' is %1
        : "rax", "rdi", "rsi", "rdx"         // Clobber list: tells the compiler we modified these registers
    );
}

#endif