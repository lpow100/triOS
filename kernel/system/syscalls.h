#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

void isr_syscall_handler(int syscallNum, int arg1, int arg2);

#endif