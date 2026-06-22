#include "processes.h"

struct pcb processes[MAX_PROCESSES];
uint8_t queue_head = 0;
uint8_t queue_tail = 0;

uint64_t load_next_process(uint64_t old_rsp) {
    processes[queue_head].kernel_rsp = old_rsp;

    queue_head ++;
    if (queue_head == MAX_PROCESSES) queue_head = 0;
    if (queue_head == queue_tail) queue_head = 0;

    return processes[queue_head].kernel_rsp;
}

void create_process(struct TEF executable) {
    void *rsp = malloc(4096 /* 4KB */);

    struct pcb process;
    process.kernel_rsp = (uint64_t)rsp;
    process.pid = processes[queue_tail - 1].pid + 1;
    struct file_descriptor* stdin = (struct file_descriptor* )malloc(sizeof(struct file_descriptor));
    stdin->type = FILE_ISTREAM; stdin->size = 0;
    struct file_descriptor* stdout = (struct file_descriptor* )malloc(sizeof(struct file_descriptor));
    stdout->type = FILE_OSTREAM; stdout->size = 0;
    process.fd_table[0] = stdin;
    process.fd_table[1] = stdout;

    processes[queue_tail] = process;
    queue_tail ++;
}