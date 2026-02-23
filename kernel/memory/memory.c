#include "memory.h"

extern uint64_t total_physical_memory;

#define GiB 0x40000000
#define MiB 0x100000
#define TABLE_PAGE_COUNT 512

#define DESIRED_MEMORY GiB * 4

static uint32_t total_page_count;
static uint32_t table_count;

void init_paging(struct multiboot_info* multiboot_info_ptr) {
    pmm_init_huge(multiboot_info_ptr);
}