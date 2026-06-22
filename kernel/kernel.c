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
#include <storage/ustar.h>
#include <assembler/assembler.h>
#include <processes/processes.h>
#include <storage/files.h>
#include <user.h>

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

//extern uint64_t framebuffer_address;

extern uint32_t ustar_start;

uint64_t stack_top;

void kernel_main(uint64_t framebuffer_address, uint32_t multiboot_info_ptr) {

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
    asm volatile ("sti"::);

    memory_init((struct multiboot_info*)multiboot_info_ptr);

    initGraphics(framebuffer_address);
    fill(BLACK);

    uint64_t idx = 0;
    uint64_t yidx = 1;

    char *file_contents = NULL;
    int size = tar_lookup((unsigned char *)ustar_start, "shell.sef", &file_contents);

    if (size == 0 || file_contents == NULL) {
        kprintf("[ERROR]: Could not find file 'shell.sef' in archive.\n");
        return;
    }
 
    struct disk_file *header = (struct disk_file *)file_contents;

    if ((header->magic & 0x000FFFFF) != 0x3C0DE) {
        kprintf("[ERROR]: File 'shell.sef' is not a validated Tri Executable Format binary.\n");
        return;
    }

    enum file_type type = GET_FILE_TYPE(header->magic);
    printU64((int)type);
    if (type != FILE_SEF) {
        kprintf("[ERROR]: 'shell.sef' is a valid file, but it is not an executable payload.\n");
        return;
    }

    struct TEF *executable = (struct TEF *)file_contents;
    create_process(*executable);

    jump_to_user_mode((void *)executable -> entry_point,(void *)processes[0].kernel_rsp);

    kprintf("back?\n");

    InitializeTerminal((u32Vector2){screen_width, screen_height});
    DrawTerminal();

    while (true) {
        char inp = getKeyChar(read_next_key());
        if (inp == 0) {continue;}
        AddTerminalInput(inp);
        fill(BLACK);
        DrawTerminal();
        asm volatile("hlt");
    } 
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
