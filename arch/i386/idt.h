#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDTBASE  0x800
#define IDTSIZE  0xFF
#define INTGATE  0x8E00

struct idtdesc {
	uint16_t offset0_15;
	uint16_t select;
	uint16_t type;
	uint16_t offset16_31;
} __attribute__ ((packed));

struct idtr {
	uint16_t limite;
	uint64_t base;
} __attribute__ ((packed));

extern volatile struct idtr kidtr;
extern volatile struct idtdesc kidt[IDTSIZE];

void init_idt_desc(uint16_t, uint32_t, uint16_t, volatile struct idtdesc *);
void init_idt(void);

#endif
