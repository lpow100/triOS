#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stdint.h>
#include <management/pmm.h>
#include <grub.h>

extern uint8_t _kernel_end;


struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern struct gdt_ptr gdt64_ptr;

static inline void read_gdtr(struct gdt_ptr* ptr) {
    __asm__ volatile("sgdt %0" : "=m"(*ptr));
}

// The paging in assembly is just a bootstrap, this is the real shit
void init_paging(struct multiboot_info* multiboot_info_ptr);

#endif