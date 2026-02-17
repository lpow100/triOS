#include "heap.h"
#include "memory.h"

#define HEAP_SIZE 0x800000 // value bigger than the small ass area given within other areas

heapInfo heap;

// initalizes the heap and returns the total size
int heapInit(){
    /*memory_sector* heapPage ;//= getPage(HEAP_SIZE);

    heap.addr = heapPage->base;
    heap.totalSize = heapPage->length;
    heap.freeSize = heapPage->length;
    heapSegment temp;
    temp.size = heapPage->length;
    temp.next = (heapSegment *)NO_NEXT_SEGMENT;
    temp.free = true;
    heap.head = &temp;
    return heapPage->length;*/
}

void *malloc(int size){

}

void free(void *memory);