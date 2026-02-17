#include "syscalls.h"

// Structure matching 'pusha' pushed registers order
void fileWrite(int fd);

// Your syscall handler gets pointer to this
void isr_syscall_handler(int syscallNum, int arg1, int arg2) {
    switch (syscallNum) {
        case 0: // WRITE
            switch (arg1) {
                case 0:
                    
                    break;
                
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
}
