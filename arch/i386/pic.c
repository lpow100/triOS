#include "io.h"

void init_pic(void)
{
  /* Initialisation de ICW1 */
  outb(0x20, 0x11);
  outb(0xA0, 0x11);

  /* Initialisation de ICW2 */
  outb(0x21, 0x20);	/* vecteur de depart = 32 */
  outb(0xA1, 0x28);	/* vecteur de depart = 96 */

  /* Initialisation de ICW3 */
  outb(0x21, 0x04);
  outb(0xA1, 0x02);

  /* Initialisation de ICW4 */
  outb(0x21, 0x01);
  outb(0xA1, 0x01);

  outb(0x21, 0xFC); // 11111100: Only IRQ 0 and 1 allowed
outb(0xA1, 0xFF); // Mask all Slave interrupts for now
}