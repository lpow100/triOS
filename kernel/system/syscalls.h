#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

typedef uint64_t jmp_buf;
extern jmp_buf de_stack_env;

#define SYS_STDIN  0
#define SYS_STDOUT 1
#define SYS_STDERR 2

uint64_t isr_syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, 
                             uint64_t rcx, uint64_t r8, uint64_t syscallNumber);

#endif