#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdint.h>
#include <stdbool.h>

#define MAX_SCANCODE 0x58
#define BUFFER_SIZE 256

typedef struct {
    const char* name;
    uint8_t scan_code;
} KeyMapping;

// Stores name in str
extern volatile char* key_buffer[BUFFER_SIZE];
extern volatile int head;
extern volatile int tail;

extern volatile KeyMapping keymap[];
extern volatile KeyMapping shiftKeymap[];
extern volatile KeyMapping capsLockKeymap[];

char *read_next_key();

void isr_keyboard_int(void);

#endif