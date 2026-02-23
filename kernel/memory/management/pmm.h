#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <grub.h>
#include <text.h>
#include <io.h>

void pmm_init_huge(struct multiboot_info* mbi);

void pmm_lock_huge_page(uintptr_t addr);
void pmm_free_huge_page(uintptr_t addr);

#endif