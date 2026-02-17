#include "keyboard.h"
#include "io.h"
#include "text.h"

#define INIT 0
#define WAIT_FOR_CURSOR 1

volatile char* key_buffer[BUFFER_SIZE];
volatile int head = 0;
volatile int tail = 0;


// Caps lock and Shift on keymap are basically unused
// Caps lock swaps when keydown caps lock, caps lock
volatile bool capsLockOn = false;
// Shift is only key that currently uses keyup
volatile bool lshiftDown = false;
volatile bool rshiftDown = false;

volatile KeyMapping keymap[] = {
    {"ESC", 0x01}, {"1", 0x02}, {"2", 0x03}, {"3", 0x04},
    {"4", 0x05}, {"5", 0x06}, {"6", 0x07}, {"7", 0x08},
    {"8", 0x09}, {"9", 0x0A}, {"0", 0x0B}, {"-", 0x0C},
    {"=", 0x0D}, {"BACKSPACE", 0x0E}, {"TAB", 0x0F},
    {"q", 0x10}, {"w", 0x11}, {"e", 0x12}, {"r", 0x13},
    {"t", 0x14}, {"y", 0x15}, {"u", 0x16}, {"i", 0x17},
    {"o", 0x18}, {"p", 0x19}, {"[", 0x1A}, {"]", 0x1B},
    {"ENTER", 0x1C}, {"LCONTROL", 0x1D}, {"a", 0x1E},
    {"s", 0x1F}, {"d", 0x20}, {"f", 0x21}, {"g", 0x22},
    {"h", 0x23}, {"j", 0x24}, {"k", 0x25}, {"l", 0x26},
    {";", 0x27}, {"'", 0x28}, {"`", 0x29}, {"LSHIFT", 0x2A},
    {"\\", 0x2B}, {"z", 0x2C}, {"x", 0x2D}, {"c", 0x2E},
    {"v", 0x2F}, {"b", 0x30}, {"n", 0x31}, {"m", 0x32},
    {",", 0x33}, {".", 0x34}, {"/", 0x35}, {"RSHIFT", 0x36},
    {"LALT", 0x38}, {"SPACE", 0x39}, {"CAPSLOCK", 0x3A},
    {"F1", 0x3B}, {"F2", 0x3C}, {"F3", 0x3D}, {"F4", 0x3E},
    {"F5", 0x3F}, {"F6", 0x40}, {"F7", 0x41}, {"F8", 0x42},
    {"F9", 0x43}, {"F10", 0x44}, {"F11", 0x57}, {"F12", 0x58}
};

volatile KeyMapping shiftKeymap[] = {
    {"ESC", 0x01}, {"!", 0x02}, {"@", 0x03}, {"#", 0x04},
    {"$", 0x05}, {"%", 0x06}, {"^", 0x07}, {"&", 0x08},
    {"*", 0x09}, {"(", 0x0A}, {")", 0x0B}, {"_", 0x0C},
    {"+", 0x0D}, {"BACKSPACE", 0x0E}, {"TAB", 0x0F},
    {"Q", 0x10}, {"W", 0x11}, {"E", 0x12}, {"R", 0x13},
    {"T", 0x14}, {"Y", 0x15}, {"U", 0x16}, {"I", 0x17},
    {"O", 0x18}, {"P", 0x19}, {"{", 0x1A}, {"}", 0x1B},
    {"ENTER", 0x1C}, {"LCONTROL", 0x1D}, {"A", 0x1E},
    {"S", 0x1F}, {"D", 0x20}, {"F", 0x21}, {"G", 0x22},
    {"H", 0x23}, {"J", 0x24}, {"K", 0x25}, {"L", 0x26},
    {":", 0x27}, {"\"", 0x28}, {"~", 0x29}, {"LSHIFT", 0x2A},
    {"|", 0x2B}, {"Z", 0x2C}, {"X", 0x2D}, {"C", 0x2E},
    {"V", 0x2F}, {"B", 0x30}, {"N", 0x31}, {"M", 0x32},
    {"<", 0x33}, {">", 0x34}, {"?", 0x35}, {"RSHIFT", 0x36},
    {"LALT", 0x38}, {"SPACE", 0x39}, {"CAPSLOCK", 0x3A},
    {"F1", 0x3B}, {"F2", 0x3C}, {"F3", 0x3D}, {"F4", 0x3E},
    {"F5", 0x3F}, {"F6", 0x40}, {"F7", 0x41}, {"F8", 0x42},
    {"F9", 0x43}, {"F10", 0x44}, {"F11", 0x57}, {"F12", 0x58}
};

char *getFromKeymap(uint8_t scancode, KeyMapping *map) {
    for (int i = 0; i < MAX_SCANCODE + 1; i++) {
        if (map[i].scan_code == scancode) {
            return map[i].name;
        }
    }
    return (char*)0xBAADF00D; // not found
}

void add_to_buffer(uint8_t scancode) {
    if (scancode == 0x2A) {
        lshiftDown = true;
        return;
    } if (scancode == 0x36) {
        rshiftDown = true;
        return;
    }
    if (scancode == 0xAA) {
        lshiftDown = false;
        return;
    } if (scancode == 0xB6) {
        rshiftDown = false;
        return;
    }

    if (scancode == 0x3A) {
        capsLockOn = !capsLockOn;
        return; // No character added for toggle itself
    }
    bool doShift = lshiftDown || rshiftDown;

    static char key_temp[256];
    char *key = key_temp;
    if (doShift) key = getFromKeymap(scancode,shiftKeymap);
    else key = getFromKeymap(scancode,keymap);

    if (key == (char*)0xBAADF00D) return;

    if (strlen(key) == 1 && capsLockOn) {
        if (key[0] >= 0x41 && key[0] <= 0x5A) key[0] += 0x20;
        else if (key[0] >= 0x61 && key[0] <= 0x7A) key[0] -= 0x20;
    }

    key_buffer[tail] = key;
    tail += 1;
    if (tail >= BUFFER_SIZE) tail = 0;
}

char *read_next_key() {
    if (head == tail) return (char*)0xBAADF00D;

    static char key_temp[256];
    char *key = key_temp;

    key = key_buffer[head];
    head += 1;
    if (head >= BUFFER_SIZE) head = 0;

    return key;
}

void isr_keyboard_int(void) {
    static int state = INIT;
    uint8_t scan_code;
    scan_code = inb(0x60);
    if (scan_code == 0xE0) {
      state = WAIT_FOR_CURSOR;      
      return;
    }
    if (state == WAIT_FOR_CURSOR) {
      //we dont use these YET
    }
    else {
        add_to_buffer(scan_code);
    }
    state = INIT;
}