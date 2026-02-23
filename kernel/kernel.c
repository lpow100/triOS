#include "text.h"
#include <timer/timer.h>
#include <keyboard/keyboard.h>
#include <idt.h>
#include "shell.h"
#include "memory.h"
#include "heap.h"
#include <video/graphics.h>
#include <video/fonts.h>
#include <gdt.h>
#include <pic.h>
#include <lib.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// If your keyboard driver uses a “no key” sentinel, define it clearly:
#define NO_KEY ((char*)0xBAADF00D)

void set_cursor(int x, int y) {
    uint16_t pos = (uint16_t)(y * 80 + x); // 80 cols per row in VGA text mode
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF)); 
}

extern uint64_t framebuffer_address;
extern uint32_t screen_width;
extern uint32_t screen_height;
extern uint32_t bits_per_pixel;
extern uint32_t pitch;

static uint32_t *framebuffer = NULL;
static uint32_t *back_buffer = (uint32_t*)0x04000000;

void kernel_main(uint64_t stack, uint32_t multiboot_info_ptr) {
    kprintf("start.s finished\n");
    uint32_t* fb = (uint32_t*)(uintptr_t)framebuffer_address;

    char bitmap_start_buff[20];
    u64AsString((uint64_t)stack,bitmap_start_buff);
    kprintf("stack: ");
    kprintf(bitmap_start_buff);
    kprintf("\n");

    asm volatile (
        "xor %%rax, %%rax\n\t"   // Zero out RAX
        "mov %%ax, %%ss\n\t"    // In 64-bit, SS can often be NULL (0)
        "mov %%ax, %%ds\n\t"    // Clear other data segments
        "mov %%ax, %%es\n\t"
        : : : "rax"       // Tell the compiler we modified these
    );
    init_idt();
    init_pic();
    init_timer();
    kprintf("Timer initialized\n");
    asm volatile ("sti"::);
    kprintf("Intterupts resumed\n");

    init_paging((struct multiboot_info*)multiboot_info_ptr);

    //framebuffer = (volatile uint32_t *)(uintptr_t)framebuffer_address;
    //kprintf("frame buffered\n");
    //for (uint32_t i = 0; i < 10; i++)
    //{
    //    framebuffer[i] = 0x0000FFFF;
    //}
    //kprintf("frame colored\n");
/*

    struct multiboot_info *mbi = (struct multiboot_info*)multiboot_info_ptr;
    (void)mbi; // suppress unused warning if you’re not using it yet

    

    memInit(magic, multiboot_info_ptr);
    kprintf("Memory online\n");

    // Initialize terminal output (assumes these globals are defined elsewhere)
    terminalInitialize(&globalTerminal, screenWidth, screenHeight);
    terminalWrite(&globalTerminal, "triOS - TEXT INIT\n");
    kprintf("Terminal online\n");

    init_gdt();
    // /* Initialisation du pointeur de pile %esp 
    asm volatile ("movw $0x18, %ax\n\t"
                  "movw %ax, %ss\n\t"
                  "movl $0x20000, %esp\n\t");
    init_idt();
    init_pic();
    init_timer();
    asm volatile ("sti"::);
    // int ticks = 0;
    // initShell();
        fill(BLACK);
        terminalDraw(&globalTerminal, 0, 0);

    while (1) {
        char *inp = read_next_key();

        // Skip empty/no-key states
        if (inp == NULL || inp == NO_KEY) {
            asm volatile("hlt");
            continue;
        }

        // Multi-char tokens like "SPACE", "BACKSPACE", "ENTER"
        if (strlen(inp) > 1) {
            if (strcmp(inp, "SPACE")) {
                terminalWrite(&globalTerminal, " ");
            } else if (strcmp(inp, "BACKSPACE")) {
                // Guard the indices; adjust to your terminal’s API
                if (globalTerminal.inputTail > globalTerminal.inputHead) {
                    globalTerminal.inputTail--;
                    globalTerminal.buffer[globalTerminal.inputTail] = '\0';
                }
            } else if (strcmp(inp, "ENTER")) {
                terminalWrite(&globalTerminal, "\n");
                // If the function is really spelled “analayzeInput”, keep it.
                // Otherwise fix the spelling in both declaration and definition.
                analayzeInput(&globalTerminal);
            }
        } else {
            // Single character
            terminalWrite(&globalTerminal, inp);
        }

        // Redraw the screen each iteration
        fill(BLACK);
        terminalDraw(&globalTerminal, 0, 0);

        asm volatile("hlt"); // Halt CPU until next inhterrupt
    }*/
}
