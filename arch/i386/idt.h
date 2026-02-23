#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDTBASE  0x800
#define IDTSIZE  0xFF
#define INTGATE  0x8E00

struct idtdesc {
    uint16_t offset0_15;   // Low 16 bits of address
    uint16_t select;       // Code segment selector (0x08)
    uint16_t type;         // Attributes (Type and DPL)
    uint16_t offset16_31;  // Middle 16 bits of address
    uint32_t offset32_63;  // High 32 bits of address
    uint32_t reserved;     // Must be 0
} __attribute__((packed));

// 64-bit IDTR (Pointer to the IDT)
struct idtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern volatile struct idtr kidtr;
extern volatile struct idtdesc kidt[IDTSIZE];

void init_idt_desc(uint16_t, uint64_t, uint16_t, volatile struct idtdesc *);
void init_idt(void);

#endif
