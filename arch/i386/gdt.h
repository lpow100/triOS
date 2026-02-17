#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define GDTBASE    0x0		/* addr. physique ou doit resider la gdt */
#define GDTSIZE    0xFF		/* nombre max. de descripteurs dans la table */

/* Descripteur de segment */
struct gdtdesc {
	uint16_t lim0_15;
	uint16_t base0_15;
	uint8_t base16_23;
	uint8_t acces;
	uint8_t lim16_19:4;
	uint8_t other:4;
	uint8_t base24_31;
} __attribute__ ((packed));

/* Registre GDTR */
struct gdtr {
	uint16_t limite;
	uint32_t base;
} __attribute__ ((packed));

/* extern struct gdtdesc kgdt[]; */
/* extern struct gdtr kgdtr; */

void init_gdt_desc(uint32_t, uint32_t, uint8_t, uint8_t, volatile struct gdtdesc *);
void init_gdt(void);

#endif