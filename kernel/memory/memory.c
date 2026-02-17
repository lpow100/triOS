#include "memory.h"
#include <text.h>
#include <io.h>

#define PAGE_ADDR_START  31
#define PAGE_AVL_START   11
#define PAGE_SIZE        7
#define PAGE_AVAILABLE   6
#define PAGE_ACCESSED    5
#define PAGE_CACHE_OFF   4
#define PAGE_CACHE_WRITE 3
#define PAGE_USER_SUPER  2
#define PAGE_READ_WRITE  1
#define PAGE_IS_PRESENT  0

extern uint64_t _kernel_end; // From linker script
uint64_t* bitmap;
uint64_t total_frames;

void bitmap_clear(uint64_t frame) {
    bitmap[frame / 64] &= ~(1ULL << (frame % 64));
}

void bitmap_set(uint64_t frame) {
    bitmap[frame / 64] |= (1ULL << (frame % 64));
}

void memInit(uint32_t magic, uint32_t mb_ptr) {
    multiboot_info_t* mb_info = (multiboot_info_t*)(uintptr_t)mb_ptr;
    
    // 1. Calculate max memory
    uint64_t max_addr = 0;
    memory_map_entry_t* mmap = (memory_map_entry_t*)(uintptr_t)mb_info->mmap_addr;
    uint32_t entries = mb_info->mmap_length / sizeof(memory_map_entry_t);

    kprintf("Memory Variables\n");
    
    
    for (uint32_t i = 0; i < entries; i++) {
        if (mmap[i].type == 1) {
            uint64_t end = mmap[i].addr + mmap[i].len;
            if (end > max_addr) max_addr = end;
        }
    }

    total_frames = max_addr / PAGE_SIZE;

    kprintf("Memory Maxxxed\n");
    
    // 2. Place bitmap at _kernel_end
    bitmap = (uint64_t*)&_kernel_end;
    uint64_t bitmap_size = (total_frames / 8); // size in bytes
    
    // Mark everything as USED (Reserved)
    for (uint64_t i = 0; i < total_frames / 64; i++) {
        bitmap[i] = 0xFFFFFFFFFFFFFFFF; 
    }

    kprintf("Placed bitmap and marked used\n");

    // 3. Clear (Free) available regions
    for (uint32_t i = 0; i < entries; i++) {
        if (mmap[i].type == 1) {
            for (uint64_t addr = mmap[i].addr; addr < mmap[i].addr + mmap[i].len; addr += PAGE_SIZE) {
                bitmap_clear(addr / PAGE_SIZE);
            }
        }
    }

    kprintf("Freed Regions\n");

    // 4. SELF-PRESERVATION: Re-reserve the kernel and the bitmap
    // Reserve from 0x0 to (kernel_end + bitmap_size)
    uint64_t kernel_and_bitmap_end = (uint64_t)bitmap + bitmap_size;
    for (uint64_t addr = 0; addr < kernel_and_bitmap_end; addr += PAGE_SIZE) {
        bitmap_set(addr / PAGE_SIZE);
    }
    kprintf("Reserve 2\n");
}