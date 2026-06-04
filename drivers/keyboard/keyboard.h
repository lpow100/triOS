#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdint.h>
#include <stdbool.h>

#define MAX_SCANCODE 0x58
#define BUFFER_SIZE 256

typedef struct {
    char ch;        // ascii character, 0 if non-printable
    uint8_t keycode;// scancode
} KeyMapping;

struct Key{    
    uint8_t keycode; 
    bool shift;
    bool ctrl;
    bool alt;
    bool capslck;
};


// Stores name in str
extern struct Key key_buffer[BUFFER_SIZE];
extern int head;
extern int tail;

extern KeyMapping keymap[256];
extern KeyMapping shiftKeymap[256];

struct Key read_next_key();
char getKeyChar(struct Key key);

void isr_keyboard_int(void);

#endif