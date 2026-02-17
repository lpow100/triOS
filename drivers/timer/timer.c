#include "timer.h"

#include <stdbool.h>

volatile uint32_t __counter = 0;

enum
  {
    cmos_address = 0x70,
    cmos_data    = 0x71
  };

static uint8_t CMOS_BCD = false;

static uint8_t get_RTC_register(int reg)
{
  outb(cmos_address, reg);
  return inb(cmos_data);
}

void init_timer(void)
{
  outb(0x43, 0x36);
  /* outb(0x40, 0x9C); */
  /* outb(0x40, 0x2E); */
  outb(0x40, 0xA9);
  outb(0x40, 0x04);
  if ((get_RTC_register(0x8B) & 4) == 0) CMOS_BCD = true;
}

void isr_clock_int(void)
{
  ++__counter;
}

void wait(uint32_t t)
{  
  uint32_t start = __counter;
  while (start + t > __counter);
}
 
static uint8_t get_update_in_progress_flag()
{
  outb(cmos_address, 0x8A);
  return (inb(cmos_data) & 0x80);
}
 
uint8_t second(void)
{
  uint8_t second, last_second;
  while (get_update_in_progress_flag());
  second = get_RTC_register(0);
  do
    {
      last_second = second;
      while (get_update_in_progress_flag());
      second = get_RTC_register(0);
    } while (last_second != second);
  if (CMOS_BCD) return ((second & 0xF0) >> 1) + ((second & 0xF0) >> 3) + (second & 0xf);
  return second;
}