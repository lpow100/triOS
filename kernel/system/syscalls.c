#include "syscalls.h"
#include <text.h>
#include <shell.h>
#include <lib.h>
#include <heap.h>
#include <system/setjmp.h>

uint64_t sys_write(uint64_t fd, char* buf, uint64_t size) {
    if (size == 0 || buf == NULL) {
        return 0; 
    }
    kprintf("fd: ");
    printU64(fd);
    kprintf("\n");

    if (fd == 1) {
        char *temp = malloc(size + 1);
        memcpy(temp, buf, size);
        temp[size] = '\0';
        AddTerminalText(temp);
        free(temp);
    } else {
        return 1; // filesystem not supported
    }

    return 0;
}

void sys_exit(uint64_t status) {
    kprintf("\nProgram exited with status code: ");
    printU64(status);
    kprintf("\n");

    // Clear up any transient buffers here if necessary
    
    // Jump completely back out to the stable kernel execution frame
    //longjmp(de_stack_env, 1);
}

// rdi, rsi, rdx, rcx, r8 are the user's arguments
// syscallNumber is what we moved into r9 in assembly
uint64_t isr_syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, 
                             uint64_t rcx, uint64_t r8, uint64_t syscallNumber) {
    switch (syscallNumber) {
        case 2: // SYS_WRITE
            return sys_write(rdi, (const char*)rsi, rdx);
            break;
            
        case 3: // SYS_READ
            //return sys_read(rdi, (char*)rsi, rdx);
            break;

        case 1:
            sys_exit(rdi);
            break;

        default:
            return -1; // Unknown syscall
    }
}