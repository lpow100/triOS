#include "keyboard.h"
#include "io.h"
#include "text.h"

#define INIT 0
#define WAIT_FOR_CURSOR 1

struct Key key_buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;


volatile bool capsLockOn = false;

volatile bool lshiftDown = false;
volatile bool rshiftDown = false;

volatile bool lctrlDown = false;
volatile bool rctrlDown = false;

volatile bool laltDown = false;
volatile bool raltDown = false;

#define L_SHIFT_KEYCODE_DOWN             0x2A
#define R_SHIFT_KEYCODE_DOWN             0x36

#define L_CTRL_KEYCODE_DOWN              0x1D
#define SECOND_BYTE_R_CTRL_KEYCODE_DOWN  0x1D

#define L_ALT_KEYCODE_DOWN               0x38
#define SECOND_BYTE_R_ALT_KEYCODE_DOWN   0x38

#define CAPS_LOCK_KEYCODE                0x3A


#define L_SHIFT_KEYCODE_UP               0xAA
#define R_SHIFT_KEYCODE_UP               0xB6

#define L_CTRL_KEYCODE_UP                0x9D
#define SECOND_BYTE_R_CTRL_KEYCODE_UP    0x9D

#define L_ALT_KEYCODE_UP                 0xB8
#define SECOND_BYTE_R_ALT_KEYCODE_UP     0xB8

KeyMapping keymap[256] = {
    [0x01] = {27,   0x01},
    [0x02] = {'1',  0x02},
    [0x03] = {'2',  0x03},
    [0x04] = {'3',  0x04},
    [0x05] = {'4',  0x05},
    [0x06] = {'5',  0x06},
    [0x07] = {'6',  0x07}, 
    [0x08] = {'7',  0x08},
    [0x09] = {'8',  0x09},
    [0x0A] = {'9',  0x0A},
    [0x0B] = {'0',  0x0B},
    [0x0C] = {'-',  0x0C},
    [0x0D] = {'=',  0x0D},
    [0x0E] = {'\b', 0x0E}, // BACKSPACE
    [0x0F] = {'\t', 0x0F}, // TAB
    [0x10] = {'q',  0x10},
    [0x11] = {'w',  0x11},
    [0x12] = {'e',  0x12},
    [0x13] = {'r',  0x13},
    [0x14] = {'t',  0x14},
    [0x15] = {'y',  0x15},
    [0x16] = {'u',  0x16},
    [0x17] = {'i',  0x17},
    [0x18] = {'o',  0x18},
    [0x19] = {'p',  0x19},
    [0x1A] = {'[',  0x1A},
    [0x1B] = {']',  0x1B},
    [0x1C] = {'\n', 0x1C}, // ENTER
    [0x1E] = {'a',  0x1E},
    [0x1F] = {'s',  0x1F},
    [0x20] = {'d',  0x20},
    [0x21] = {'f',  0x21},
    [0x22] = {'g',  0x22},
    [0x23] = {'h',  0x23},
    [0x24] = {'j',  0x24},
    [0x25] = {'k',  0x25},
    [0x26] = {'l',  0x26},
    [0x27] = {';',  0x27},
    [0x28] = {'\'', 0x28},
    [0x29] = {'`',  0x29},
    [0x2B] = {'\\', 0x2B},
    [0x2C] = {'z',  0x2C},
    [0x2D] = {'x',  0x2D},
    [0x2E] = {'c',  0x2E},
    [0x2F] = {'v',  0x2F},
    [0x30] = {'b',  0x30},
    [0x31] = {'n',  0x31},
    [0x32] = {'m',  0x32},
    [0x33] = {',',  0x33},
    [0x34] = {'.',  0x34},
    [0x35] = {'/',  0x35},
    [0x39] = {' ',  0x39}, // SPACE
};

KeyMapping shiftKeymap[256] = {
    [0x02] = {'!',  0x02},
    [0x03] = {'@',  0x03},
    [0x04] = {'#',  0x04},
    [0x05] = {'$',  0x05},
    [0x06] = {'%',  0x06},
    [0x07] = {'^',  0x07},
    [0x08] = {'&',  0x08},
    [0x09] = {'*',  0x09},
    [0x0A] = {'(',  0x0A},
    [0x0B] = {')',  0x0B},
    [0x0C] = {'_',  0x0C},
    [0x0D] = {'+',  0x0D},
    [0x0E] = {'\b', 0x0E},
    [0x0F] = {'\t', 0x0F},
    [0x10] = {'Q',  0x10},
    [0x11] = {'W',  0x11},
    [0x12] = {'E',  0x12},
    [0x13] = {'R',  0x13},
    [0x14] = {'T',  0x14},
    [0x15] = {'Y',  0x15},
    [0x16] = {'U',  0x16},
    [0x17] = {'I',  0x17},
    [0x18] = {'O',  0x18},
    [0x19] = {'P',  0x19},
    [0x1A] = {'{',  0x1A},
    [0x1B] = {'}',  0x1B},
    [0x1C] = {'\n', 0x1C},
    [0x1E] = {'A',  0x1E},
    [0x1F] = {'S',  0x1F},
    [0x20] = {'D',  0x20},
    [0x21] = {'F',  0x21},
    [0x22] = {'G',  0x22},
    [0x23] = {'H',  0x23},
    [0x24] = {'J',  0x24},
    [0x25] = {'K',  0x25},
    [0x26] = {'L',  0x26},
    [0x27] = {':',  0x27},
    [0x28] = {'"',  0x28},
    [0x29] = {'~',  0x29},
    [0x2B] = {'|',  0x2B},
    [0x2C] = {'Z',  0x2C},
    [0x2D] = {'X',  0x2D},
    [0x2E] = {'C',  0x2E},
    [0x2F] = {'V',  0x2F},
    [0x30] = {'B',  0x30},
    [0x31] = {'N',  0x31},
    [0x32] = {'M',  0x32},
    [0x33] = {'<',  0x33},
    [0x34] = {'>',  0x34},
    [0x35] = {'?',  0x35},
    [0x39] = {' ',  0x39},
};

char getKeyChar(struct Key key) {
    KeyMapping keymapping = key.shift ? shiftKeymap[key.keycode] : keymap[key.keycode];

    if (keymapping.ch != 0) return keymapping.ch;

    if (key.capslck) {
        if (keymapping.ch >= 0x41 && keymapping.ch <= 0x5A) keymapping.ch += 0x20; else if (keymapping.ch >= 0x61 && keymapping.ch <= 0x7A) keymapping.ch -= 0x20;
    }
    if (key.ctrl) {
        if (keymapping.ch >= 'a' && keymapping.ch <= 'z') return keymapping.ch - 'a' + 1;
        if (keymapping.ch >= 'A' && keymapping.ch <= 'Z') return keymapping.ch - 'A' + 1;

        // what brought the user to bring the code here?
        // Are they trying ctrl + 1????
        return 0;
    }

    return 0;
}

void add_to_buffer(uint8_t scancode) {
    struct Key key = { 0 };

    key.keycode = scancode;
    key.shift = lshiftDown || rshiftDown;
    key.ctrl = lctrlDown || rctrlDown;
    key.alt = laltDown || raltDown;
    key.capslck = capsLockOn;

    key_buffer[tail] = key;
    tail += 1;
    if (tail >= BUFFER_SIZE) tail = 0;
}

struct Key read_next_key() {
    if (head == tail) return (struct Key) { 0 };

    struct Key key = { 0 };
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
        if (scan_code == SECOND_BYTE_R_CTRL_KEYCODE_DOWN) {
            rctrlDown = true;
        } else if (scan_code == SECOND_BYTE_R_CTRL_KEYCODE_UP) {
            rctrlDown = false;
        } else if (scan_code == SECOND_BYTE_R_ALT_KEYCODE_DOWN) {
            raltDown = true;
        } else if (scan_code == SECOND_BYTE_R_ALT_KEYCODE_UP) {
            raltDown = false;
        }
    }
    else {
        if (scan_code == L_SHIFT_KEYCODE_DOWN) {
            lshiftDown = true;
        } else if (scan_code == L_SHIFT_KEYCODE_UP) {
            lshiftDown = false;
        } else if (scan_code == R_SHIFT_KEYCODE_DOWN) {
            rshiftDown = true;
        } else if (scan_code == R_SHIFT_KEYCODE_UP) {
            rshiftDown = false;
        } else if (scan_code == L_CTRL_KEYCODE_DOWN) {
            lctrlDown = true;
        } else if (scan_code == L_CTRL_KEYCODE_UP) {
            lctrlDown = false;
        } else if (scan_code == L_ALT_KEYCODE_DOWN) {
            laltDown = true;
        } else if (scan_code == L_ALT_KEYCODE_UP) {
            laltDown = false;
        } else if (scan_code == CAPS_LOCK_KEYCODE) {
            capsLockOn = !capsLockOn;
        } else add_to_buffer(scan_code);
    }
    state = INIT;
}