#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stdbool.h>

#define NO_NEXT_SEGMENT 0xBAADF00D

typedef struct heapSegment {
    uint32_t size;
    struct heapSegment* next;
    bool free;
}__attribute__((packed)) heapSegment;


typedef struct {
    uint64_t addr;
    uint32_t freeSize;
    uint32_t totalSize;
    heapSegment* head;
}__attribute__((packed)) heapInfo;

int heapInit();

void *malloc(int size);

void free(void *memory);

#endif