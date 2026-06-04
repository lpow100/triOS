#include "pmm.h"

#define HUGE_PAGE_SIZE 0x200000 

extern uint8_t _kernel_end;
extern uint64_t total_physical_memory;

static volatile uint8_t* mem_bitmap;
static uint64_t total_huge_pages;

extern uint8_t pmm_bitmap_start;
extern uint8_t pmm_bitmap_end;

extern uint32_t mmap_addr;

void pmm_init_huge(struct multiboot_info* mbi) {
    total_huge_pages = total_physical_memory / HUGE_PAGE_SIZE;
    uint32_t bitmap_size_bytes = (total_huge_pages + 7) / 8;

    mem_bitmap = (uint8_t*)&pmm_bitmap_start;

    char bitmap_start_buff[20];
    u64AsString((uint64_t)mem_bitmap,bitmap_start_buff);
    
    for (uint32_t i = 0; i < bitmap_size_bytes; i++) {
        mem_bitmap[i] = 0xFF;
    }

    struct multiboot_mmap* mmap = (struct multiboot_mmap*)mmap_addr;
    uint64_t mmap_end = (uint64_t)mmap + mmap->size;    
    struct multiboot_mmap_entry* curr_entry = (struct multiboot_mmap_entry*)((uintptr_t)mmap + sizeof(struct multiboot_mmap));

    while ((uintptr_t)curr_entry < mmap_end) {
        if (curr_entry->type == 1) {
            for (uint64_t addr = curr_entry->base_addr; addr + HUGE_PAGE_SIZE <= curr_entry->base_addr + curr_entry->length; addr += HUGE_PAGE_SIZE) {
                pmm_free_huge_page(addr);
            }
        }
        curr_entry = (struct multiboot_mmap_entry*)((uintptr_t)curr_entry + mmap->entry_size);
    }

    uintptr_t kernel_limit = (uintptr_t)mem_bitmap + bitmap_size_bytes;
    for (uintptr_t addr = 0; addr < kernel_limit; addr += HUGE_PAGE_SIZE) {
        pmm_lock_huge_page(addr);
    }
}

uintptr_t pmm_alloc_huge_page() {
    for (uint64_t i = 0; i < total_huge_pages; i++) {
        if (!(mem_bitmap[i / 8] & (1 << (i % 8)))) {
            mem_bitmap[i / 8] |= (1 << (i % 8));
            return i * HUGE_PAGE_SIZE;
        }
    }
    return 0; // out of memory
}

void pmm_lock_huge_page(uintptr_t addr) {
    uint64_t page = addr / HUGE_PAGE_SIZE;
    mem_bitmap[page / 8] |= (1 << (page % 8));
}

void pmm_free_huge_page(uintptr_t addr) {
    uint64_t page = addr / HUGE_PAGE_SIZE;
    mem_bitmap[page / 8] &= ~(1 << (page % 8));
} 