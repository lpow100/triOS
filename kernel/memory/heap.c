#include "heap.h"
#include "memory.h"
#include "io.h"
#include <text.h>

#define HEAP_SIZE 0x800000 // value bigger than the small ass area given within other areas

struct heapInfo heap;

int findHeapRegion(struct multiboot_info* multiboot_info_ptr) {
    struct multiboot_mmap_entry* mmap = (struct multiboot_mmap_entry*)multiboot_info_ptr->mmap_addr;
    uint64_t mmap_end = multiboot_info_ptr->mmap_addr + multiboot_info_ptr->mmap_length;

    uintptr_t kernel_limit = (uintptr_t)&_kernel_end;

    uint64_t heap_addr;

    kprintf("variables setup\n");

    char lenbuff[20];
    u64AsString(multiboot_info_ptr->mmap_addr,lenbuff);
    kprintf(lenbuff);
    kprintf("\n");

    while((uintptr_t) mmap < mmap_end) {
        kprintf("attempting heap region find\n");
        uint64_t start = mmap->addr;
        uint64_t end = start + mmap->size;

        if (start < kernel_limit) {
            start = kernel_limit;
        }

        if (mmap->type != 1 || end <= start /* prevents integer underflow from next check */ || end - start < HEAP_SIZE) {
            mmap = (struct multiboot_mmap_entry*)((uintptr_t)mmap + mmap->size + sizeof(mmap->size));
            continue;
        }
    
        heap_addr = start;
        return heap_addr;
    }

    kprintf("heap setup\n");
}

// initalizes the heap and returns the total size
int heapInit(struct multiboot_info* multiboot_info_ptr) {
    uint64_t addr = findHeapRegion(multiboot_info_ptr);
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

void *malloc(int size) {
    // 1. Align size for 64-bit (16-byte alignment is best)
    size = (size + 15) & ~15;

    struct heapSegment *curr = heap.free; // Start at the beginning of the free list

    while (curr != BAD_SEGMENT_PTR) {
        // We need 'size' for data + 'sizeof(struct heapSegment)' for the NEXT header if we split
        if (curr->size < size) {
            curr = curr->next;
            continue;
        }
            
        // Fragmentation
        adjustFreeList(curr, size);

        heap.freeSize -= (curr->size + sizeof(struct heapSegment));
        
        // Return the address AFTER the header
        curr->magic = ALLOCATED_SEG;
        return (void*)((uint8_t*)curr + sizeof(struct heapSegment));
    }
    return (void*)0; 
}

void free(void *memory) {
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