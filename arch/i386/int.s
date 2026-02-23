.intel_syntax noprefix
.section .text
.align 4

.extern isr_default_int
.extern isr_clock_int
.extern isr_keyboard_int
.extern isr_syscall_handler

.global _asm_default_int
.global _asm_irq_0
.global _asm_irq_1
.global _asm_syscall_int
.global _asm_pf

.macro PUSH_REGS
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
.endm

.macro POP_REGS
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
.endm


_asm_default_int:
    PUSH_REGS

    mov rdi, rsp              
    call isr_default_int

    mov al, 0x20
    out 0x20, al              

    POP_REGS
    iretq

_asm_pf:
    PUSH_REGS

    mov al, 80
    mov dx, 0x3F8
    out dx, al

    mov rdi, rsp              
    call isr_default_int

    mov al, 0x20
    out 0x20, al              

    POP_REGS
    iretq


_asm_irq_0:
    PUSH_REGS

    mov rdi, rsp
    call isr_clock_int

    mov al, 0x20
    out 0x20, al

    POP_REGS
    iretq


_asm_irq_1:
    PUSH_REGS

    mov rdi, rsp
    call isr_keyboard_int

    mov al, 0x20
    out 0x20, al

    POP_REGS
    iretq


_asm_syscall_int:
    PUSH_REGS

    mov rdi, rsp              
    call isr_syscall_handler

    mov al, 0x20
    out 0x20, al

    POP_REGS
    iretq
