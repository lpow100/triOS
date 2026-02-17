#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stddef.h>

// Function to read a byte from an I/O port
uint8_t inb(uint16_t port);

// Function to write a byte to an I/O port
void outb(uint16_t port, uint8_t value);

// Function for a small delay
void io_wait(void);

static inline void serial_putc(char c);

void kprintf(const char* str);

#endif