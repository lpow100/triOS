.section .text
.align 4

.extern isr_default_int
.extern isr_clock_int
.extern isr_keyboard_int
.extern isr_syscall_handler

.extern load_next_process

.global _asm_default_int
.global _asm_irq_0
.global _asm_irq_1
.global _asm_syscall_int
.global _asm_pf

.macro PUSH_REGS
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    pushq %rax
.endm

.macro POP_REGS
    popq %rax
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
.endm

_asm_default_int:
    PUSH_REGS

    movq %rsp, %rdi              
    call isr_default_int

    movb $0x20, %al
    outb %al, $0x20              

    POP_REGS
    iretq

_asm_pf:
    PUSH_REGS

    movb $80, %al
    movw $0x3F8, %dx
    outb %al, (%dx)

    movq %rsp, %rdi              
    call isr_default_int

    movb $0x20, %al
    outb %al, $0x20              

    POP_REGS
    iretq

_asm_irq_0:
    PUSH_REGS

    movq %rsp, %rdi
    call isr_clock_int

    movb $0x20, %al
    outb %al, $0x20

    POP_REGS
    iretq

_asm_irq_1:
    PUSH_REGS

    movq %rsp, %rdi
    call isr_keyboard_int

    movb $0x20, %al
    outb %al, $0x20

    POP_REGS
    iretq

_asm_syscall_int:
    PUSH_REGS

    movq %rax, %r9  
    subq $8, %rsp
    call isr_syscall_handler
    addq $8, %rsp

    mov %rsp, %rdi
    call load_next_process

    movq %rax, %rsp

    POP_REGS

    iretq