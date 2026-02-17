#include "gdt.h"
#include <text.h>

#define GDTSIZE 5

volatile struct gdtdesc kgdt[GDTSIZE];
volatile struct gdtr kgdtr;

void init_gdt_desc(uint32_t base, uint32_t limit, uint8_t access, uint8_t other, volatile struct gdtdesc *desc) {
    desc->lim0_15 = (limit & 0xffff);
    desc->base0_15 = (base & 0xffff);
    desc->base16_23 = (base & 0xff0000) >> 16;
    desc->acces = access;
    desc->lim16_19 = (limit & 0xf0000) >> 16;
    desc->other = (other & 0xf);
    desc->base24_31 = (base & 0xff000000) >> 24;
}

void init_gdt(void) {
    /* 1. Null Descriptor */
    init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);

    /* 2. Kernel Code Segment 
       Access: 0x9A (Present, Ring 0, Exec/Read)
       Other:  0x2  (L-bit set for 64-bit mode) */
    init_gdt_desc(0x0, 0, 0x9A, 0x2, &kgdt[1]);

    /* 3. Kernel Data Segment
       Access: 0x92 (Present, Ring 0, Read/Write)
       Other:  0x0 */
    init_gdt_desc(0x0, 0, 0x92, 0x0, &kgdt[2]);

    /* 4. Setup the GDTR pointer */
    kgdtr.limite = (uint16_t)(GDTSIZE * sizeof(struct gdtdesc) - 1);
    kgdtr.base = (uintptr_t)kgdt;

    /* 5. Load GDT and perform a "Far Return" to reload CS */
    asm volatile (
        "lgdt (%0)             \n\t"
        "pushq $0x08           \n\t" // Push code selector (Entry 1 * 8)
        "leaq  1f(%%rip), %%rax \n\t" // Get address of label '1'
        "pushq %%rax           \n\t" // Push RIP
        "lretq                 \n\t" // Long Return: pops RIP then CS
        "1:                    \n\t"
        "mov $0x10, %%ax       \n\t" // Data selector (Entry 2 * 8)
        "mov %%ax, %%ds        \n\t"
        "mov %%ax, %%es        \n\t"
        "mov %%ax, %%fs        \n\t"
        "mov %%ax, %%gs        \n\t"
        "mov %%ax, %%ss        \n\t"
        : : "r"(&kgdtr) : "rax", "memory"
    );
}