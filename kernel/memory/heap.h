#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>
#include "grub.h"

#define BAD_SEGMENT_PTR (struct heapSegment*) 0xBAADF00D
#define ALLOCATED_SEG 0x414C4F43
#define FREE_SEG 0x46524545

struct heapSegment {
    uint32_t magic; // e.g., 0x414C4F43 ('ALOC') or 0x46524545 ('FREE')
    uint32_t size;
    struct heapSegment* next;
    struct heapSegment* prev;
}__attribute__((packed));

struct heapInfo{
    uint64_t addr;
    uint32_t freeSize;
    uint32_t totalSize;
    struct heapSegment* free;
}__attribute__((packed));

extern struct heapInfo heap;

int heapInit(struct multiboot_info* multiboot_info_ptr);

void *malloc(int size);

void free(void *memory);

#endif