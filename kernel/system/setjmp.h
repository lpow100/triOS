#ifndef SETJMP_H
#define SETJMP_H

#include <stdint.h>

// Array indexes for our register storage buffer
#define JB_RBX 0
#define JB_RBP 1
#define JB_R12 2
#define JB_R13 3
#define JB_R14 4
#define JB_R15 5
#define JB_RSP 6
#define JB_RIP 7

// A jmp_buf is simply an array of 8 64-bit integers holding the register states
typedef uint64_t jmp_buf;

// Function declarations for our assembly implementations
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

#endif