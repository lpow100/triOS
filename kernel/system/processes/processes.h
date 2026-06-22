#ifndef PROCESSES_H
#define PROCESSES_H

#include <stdint.h>
#include <storage/files.h>

// while process is loaded

#define MAX_FDS 64

struct pcb {
    uint64_t kernel_rsp;
    uint64_t pid;
    struct file_descriptor* fd_table[MAX_FDS];
}; 

#define MAX_PROCESSES 64

extern struct pcb processes[MAX_PROCESSES];
extern uint8_t queue_head;
extern uint8_t queue_tail;

// returns the rsp for the process to run
uint64_t load_next_process(uint64_t old_rsp);

// before process is loaded

struct TEF {
    uint32_t magic;
    uint32_t version;
    uint64_t entry_point;    
    uint64_t text_vaddr;     
    uint64_t text_size;      
    uint64_t data_size;      
    uint64_t bss_size;
};

void create_process(struct TEF executable);

#endif