#include <stdint.h>
#include "lib.h"
#include "io.h"
#include "idt.h"

// External assembly wrappers
void _asm_default_int(void);
void _asm_irq_0(void);
void _asm_irq_1(void);
void _asm_syscall_int(void);
void _asm_pf(void);

void isr_default_int() {
    
}

volatile struct idtr kidtr;
volatile struct idtdesc kidt[IDTSIZE];

/**
 * init_idt_desc: Initializes an IDT gate descriptor.
 * @select: The code segment selector (0x08).
 * @offset: The 64-bit linear address of the ISR wrapper.
 * @type: Gate type (usually INTGATE / 0x8E00).
 * @desc: Pointer to the descriptor in the kidt array.
 */
void init_idt_desc(uint16_t select, uint64_t offset, uint16_t type, volatile struct idtdesc *desc)
{
    desc->offset0_15 = (offset & 0xFFFF);
    desc->select = select;
    desc->type = type; // Usually 0x8E00 for 64-bit interrupt gate
    desc->offset16_31 = (offset >> 16) & 0xFFFF;
    desc->offset32_63 = (offset >> 32) & 0xFFFFFFFF;
    desc->reserved = 0;
}

/**
 * init_idt: Initializes the Interrupt Descriptor Table.
 */
void init_idt(void)
{
    // Fill the table with a default "unhandled interrupt" handler
    for (int i = 0; i < IDTSIZE; i++) {
        if (i == 0xE) {
            init_idt_desc(0x08, (uint64_t)_asm_pf, 0x8E00, &kidt[i]);
        }
        init_idt_desc(0x08, (uint64_t)_asm_default_int, 0x8E00, &kidt[i]);
    }

    // Specific hardware interrupts (IRQs mapped to 32+)
    init_idt_desc(0x08, (uint64_t)_asm_irq_0, 0x8E00, &kidt[32]);      // Timer
    init_idt_desc(0x08, (uint64_t)_asm_irq_1, 0x8E00, &kidt[33]);      // Keyboard
    init_idt_desc(0x08, (uint64_t)_asm_syscall_int, 0xEE00, &kidt[128]); // Syscall (DPL 3)

    // Set the IDT pointer
    kidtr.limit = (IDTSIZE * sizeof(struct idtdesc)) - 1;
    kidtr.base = (uint64_t)&kidt; // No need for IDTBASE if we use the address of the array

    // Load the IDT into the CPU
    asm volatile ("lidt %0" : : "m" (kidtr));
}