#include "pmm.h"

#define HUGE_PAGE_SIZE 0x200000 

extern uint8_t _kernel_end;
extern uint64_t total_physical_memory;

static uint8_t* mem_bitmap;
static uint64_t total_huge_pages;

void pmm_init_huge(struct multiboot_info* mbi) {

    kprintf("pmm init begin\n");

    total_huge_pages = total_physical_memory / HUGE_PAGE_SIZE;
    uint32_t bitmap_size_bytes = (total_huge_pages + 7) / 8;

    mem_bitmap = (uint8_t*)&_kernel_end + 16384;

    char bitmap_start_buff[20];
    u64AsString((uint64_t)mem_bitmap,bitmap_start_buff);
    kprintf("bitmap start: ");
    kprintf(bitmap_start_buff);
    kprintf("\n");
    
    for (uint32_t i = 0; i < 16; i++) mem_bitmap[i] = 0xFF;

    kprintf("pmm init begin\n");

    struct multiboot_mmap_entry* mmap = (struct multiboot_mmap_entry*)mbi->mmap_addr;
    uint32_t mmap_end = mbi->mmap_addr + mbi->mmap_length;

    char mmap_start_buff[20];
    u64AsString(mbi->mmap_addr,mmap_start_buff);
    kprintf("mmap start: ");
    kprintf(mmap_start_buff);
    kprintf("\n");


    char mmap_end_buff[20];
    u64AsString(mmap_end,mmap_end_buff);
    kprintf("mmap end: ");
    kprintf(mmap_end_buff);
    kprintf("\n");

    while ((uintptr_t)mmap < mmap_end) {
        kprintf("Could enter loop!\n");
        if (mmap->type == 1) {
            for (uint64_t addr = mmap->addr; addr + HUGE_PAGE_SIZE <= mmap->addr + mmap->len; addr += HUGE_PAGE_SIZE) {
                pmm_free_huge_page(addr);
            }
        }
        mmap = (struct multiboot_mmap_entry*)((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
    }

    uintptr_t kernel_limit = (uintptr_t)mem_bitmap + bitmap_size_bytes;
    for (uintptr_t addr = 0; addr < kernel_limit; addr += HUGE_PAGE_SIZE) {
        pmm_lock_huge_page(addr);
    }
}

void pmm_lock_huge_page(uintptr_t addr) {
    uint64_t page = addr / HUGE_PAGE_SIZE;
    mem_bitmap[page / 8] |= (1 << (page % 8));
}

void pmm_free_huge_page(uintptr_t addr) {
    uint64_t page = addr / HUGE_PAGE_SIZE;
    mem_bitmap[page / 8] &= ~(1 << (page % 8));
}