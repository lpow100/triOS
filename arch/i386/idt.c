#include <stdint.h>
#include "lib.h"
#include "io.h"
#include "idt.h"

void _asm_default_int(void);
void _asm_irq_0(void);
void _asm_irq_1(void);
void _asm_syscall_int(void);

volatile struct idtr kidtr;
volatile struct idtdesc kidt[IDTSIZE];

/*
 * 'init_idt_desc' initialise un descripteur de segment situe en idt.
 * 'desc' est l'adresse lineaire du descripteur a initialiser.
 * L'argument 'type' doit avoir pour valeur INTGATE.
 */
void init_idt_desc(uint16_t select, uint32_t offset, uint16_t type, volatile struct idtdesc *desc)
{
  desc->offset0_15 = (offset & 0xffff);
  desc->select = select;
  desc->type = type;
  desc->offset16_31 = (offset & 0xffff0000) >> 16;
  return;
}

/*
 * Cette fonction initialise la IDT apres que le kernel soit charge
 * en memoire.
 */
void init_idt(void)
{  
  for (int i = 0; i < IDTSIZE; i++)
    init_idt_desc(0x08, (uint32_t) _asm_default_int, INTGATE, &kidt[i]);

  init_idt_desc(0x08, (uint32_t) _asm_irq_0, INTGATE, &kidt[32]);	/* timer */
  init_idt_desc(0x08, (uint32_t) _asm_irq_1, INTGATE, &kidt[33]);	/* keyboard */
  init_idt_desc(0x08, (uint32_t) _asm_irq_1, (uint16_t)_asm_syscall_int, &kidt[80]);	/* keyboard */

  kidtr.limite = IDTSIZE * sizeof(struct idtdesc) - 1;
  kidtr.base = IDTBASE;

  memcpy((char *) kidtr.base, (char *) kidt, IDTSIZE * sizeof(struct idtdesc));

  asm volatile ("lidt %0" : : "m" (kidtr));
}
