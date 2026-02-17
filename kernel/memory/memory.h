#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stdint.h>

#define MAX_MEM_SECTORS 32
#define MAX_PAGES 128 
//typical program stack size
typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) memory_map_entry_t;

/* Multiboot Info Structure (The part we care about) */
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) multiboot_info_t;

// when returning -1 please give me max mem sectors
void memInit(uint32_t magic, uint32_t multiboot_info_ptr);
void* alloc_frame();

/*memory_sector *getPage(int mallocSize);
void freePage(memory_sector* page);

stack getStack();*/

#endif