#include "heap.h"
#include "memory.h"
#include "io.h"
#include <text.h>

#define HEAP_SIZE 0x200000 // value bigger than the small ass area given within other areas

struct heapInfo heap;
static bool canUseHeap = false;

// initalizes the heap and returns the total size
int heapInit() {
    // TODO: Change from 2MB heap to a 8MB heap
    uint64_t addr = pmm_alloc_huge_page();
    if (addr == 0) return 0;

    heap.addr = addr;
    heap.totalSize = HEAP_SIZE;
    
    // The first header sits at the very start of the region
    struct heapSegment* first = (struct heapSegment*)addr;
    first->size = HEAP_SIZE - sizeof(struct heapSegment);
    first->next = BAD_SEGMENT_PTR;
    first->prev = BAD_SEGMENT_PTR;
    first->magic = FREE_SEG;

    heap.free = first;
    heap.freeSize = first->size;
    canUseHeap = true;

    return 0;
}

void adjustFreeList(struct heapSegment* curr, int size) {
    if (curr->size > size + sizeof(struct heapSegment) + 16) {
        // Calculate where the new 'Free' header will go
        struct heapSegment* next_free = (struct heapSegment*)((uint8_t*)curr + sizeof(struct heapSegment) + size);
        
        // Initialize the new free header
        next_free->size = curr->size - size - sizeof(struct heapSegment);
        next_free->next = curr->next;
        next_free->prev = curr->prev;

        // Update the links in the list
        if (curr->prev != BAD_SEGMENT_PTR) curr->prev->next = next_free;
        else heap.free = next_free;

        if (next_free->next != BAD_SEGMENT_PTR) next_free->next->prev = next_free;

        curr->size = size; // Current header now only tracks the allocated size
    } else {
        // Perfect fit: Just remove from the free list
        if (curr->prev != BAD_SEGMENT_PTR) curr->prev->next = curr->next;
        else heap.free = curr->next;

        if (curr->next != BAD_SEGMENT_PTR) curr->next->prev = curr->prev;
    }
}

void *malloc(size_t size) {
    if (!canUseHeap) {
        kprintf("[ERROR]: Using malloc before heap init!\n");
        return 0x0;
    }
    // 1. Align size for 64-bit (16-byte alignment is best)
    size = (size + 15) & ~15;

    struct heapSegment *curr = heap.free; // Start at the beginning of the free list

    while (curr != BAD_SEGMENT_PTR) {
        // We need 'size' for data + 'sizeof(struct heapSegment)' for the NEXT header if we split
        if (curr->size < size) {
            curr = curr->next;
            continue;
        }

        heap.freeSize -= (size + sizeof(struct heapSegment));
            
        // Fragmentation
        adjustFreeList(curr, size);
        
        // Return the address AFTER the header
        curr->magic = ALLOCATED_SEG;
        return (void*)((uint8_t*)curr + sizeof(struct heapSegment));
    }
    return (void*)0; 
}

void free(void *memory) {
    if (!canUseHeap) {
        kprintf("[ERROR]: Using free before heap init!\n");
        return;
    }
    if (!memory) return;

    struct heapSegment* curr = (struct heapSegment*)((uint8_t*)memory - sizeof(struct heapSegment));
    
    // Safety check: Magic and Bounds
    if (curr->magic != ALLOCATED_SEG) return; 

    curr->magic = FREE_SEG;
    heap.freeSize += (curr->size + sizeof(struct heapSegment));

    // 1. Insert into the sorted free list
    struct heapSegment* iter = heap.free;
    struct heapSegment* prev = BAD_SEGMENT_PTR;

    while (iter != BAD_SEGMENT_PTR && iter < curr) {
        prev = iter;
        iter = iter->next;
    }

    // Insert curr between prev and iter
    curr->next = iter;
    curr->prev = prev;

    if (prev != BAD_SEGMENT_PTR) prev->next = curr;
    else heap.free = curr;

    if (iter != BAD_SEGMENT_PTR) iter->prev = curr;

    // 2. Coalesce Right (Merge curr with next)
    if (curr->next != BAD_SEGMENT_PTR) {
        // Check if they are physically touching
        if ((uint8_t*)curr + sizeof(struct heapSegment) + curr->size == (uint8_t*)curr->next) {
            curr->size += sizeof(struct heapSegment) + curr->next->size;
            struct heapSegment* next_next = curr->next->next;
            curr->next = next_next;
            if (next_next != BAD_SEGMENT_PTR) next_next->prev = curr;
        }
    }

    // 3. Coalesce Left (Merge prev with curr)
    if (curr->prev != BAD_SEGMENT_PTR) {
        struct heapSegment* p = curr->prev;
        if ((uint8_t*)p + sizeof(struct heapSegment) + p->size == (uint8_t*)curr) {
            p->size += sizeof(struct heapSegment) + curr->size;
            p->next = curr->next;
            if (curr->next != BAD_SEGMENT_PTR) curr->next->prev = p;
        }
    }
}