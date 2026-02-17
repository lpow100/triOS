#include "io.h"

uint8_t inb(uint16_t port) {
    uint8_t ret;
    // "inb %%dx, %%al" : Reads a byte from port (in DX) into AL
    // "=a"(ret)      : Output constraint - The value in AL is put into 'ret'
    // "d"(port)      : Input constraint - The 'port' variable is moved into DX
    asm volatile ("inb %%dx, %%al" : "=a"(ret) : "d"(port));
    return ret;
}

void outb(uint16_t port, uint8_t value) {
    // "outb %%al, %%dx" : Writes 'value' (in AL) to 'port' (in DX)
    // "a"(value)       : Input constraint - The 'value' variable is moved into AL
    // "d"(port)        : Input constraint - The 'port' variable is moved into DX
    asm volatile ("outb %%al, %%dx" : : "a"(value), "d"(port));
}

void io_wait(void) {
    // Arbitrary port usually unused by other devices.
    // Reading from port 0x80 is common for this.
    asm volatile ( "jmp 1f\n\t1:jmp 2f\n\t2:" ); // Small CPU delay
    // Or a more robust way for older systems:
    // outb(0x80, 0); // Write to an unused port, causes a short delay
}

#define COM1 0x3F8

static inline void serial_putc(char c) {
    while (!(inb(COM1 + 5) & 0x20)); // wait for transmit buffer empty
    outb(COM1, c);
}

void kprintf(const char* str) {
    for (size_t i = 0; str[i]; i++)
        serial_putc(str[i]);
}