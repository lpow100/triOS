.section .multiboot_header
.align 8
header_start:
    /* magic number (multiboot 2) */
    .long 0xe85250d6
    /* architecture 0 (protected mode i386) */
    .long 0
    /* header length */
    .long header_end - header_start
    /* checksum */
    .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    /* framebuffer tag */
    .align 8
framebuffer_tag_start:
    .word 5                                    /* type = framebuffer */
    .word 0                                    /* flags */
    .long framebuffer_tag_end - framebuffer_tag_start /* size */
    .long 1280                                 /* width */
    .long 720                                  /* height */
    .long 32                                   /* depth (bits per pixel) */
framebuffer_tag_end:

    /* end tag */
    .align 8
    .word 0                                    /* type */
    .word 0                                    /* flags */
    .long 8                                    /* size */
header_end:



/*
The multiboot standard does not define the value of the stack pointer register
(esp) and it is up to the kernel to provide a stack. This allocates room for a
small stack by creating a symbol at the bottom of it, then allocating 16384
bytes for it, and finally creating a symbol at the top. The stack grows
downwards on x86. The stack is in its own section so it can be marked nobits,
which means the kernel file is smaller because it does not contain an
uninitialized stack. The stack on x86 must be 16-byte aligned according to the
System V ABI standard and de-facto extensions. The compiler will assume the
stack is properly aligned and failure to align the stack will result in
undefined behavior.
*/
.section .bss
.align 4096
p4_table:
    .skip 4096
p3_table:
    .skip 4096
p2_table_1:
    .skip 4096 * 4
p2_table_2:
    .skip 4096
p2_table_3:
    .skip 4096
p2_table_4:
    .skip 4096
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .data
no_multiboot_msg:
    .ascii "No Multiboot!\n\0"
no_cpuid_msg:
    .ascii "No cpuid!\n\0"
no_long_mode_msg:
    .ascii "Long Mode not supported on this CPU!\n\0"
no_framebuffer_msg:
    .ascii "No Framebuffer!\n\0"
test_msg:
    .ascii "test123\n\0"
fail_lme_msg:
    .ascii "fail_lme\n\0"
fail_pg_msg:
    .ascii "fail_pg\n\0"
fail_pae_msg:
    .ascii "fail_pae\n\0"
    .globl framebuffer_address
framebuffer_address:
    .long 0          # 32-bit low part of framebuffer address
    .long 0          # 32-bit high part of framebuffer address (if using 64-bit addresses)

    .globl pitch
pitch:
    .long 0

    .globl screen_width
screen_width:
    .long 0

    .globl screen_height
screen_height:
    .long 0

    .globl bits_per_pixel
bits_per_pixel:
    .long 0

    .globl total_physical_memory
total_physical_memory:
    .long 0          # low 32-bit
    .long 0          # high 32-bit for 64-bit RAM totals

    .globl magic_number
magic_number:
    .long 0



/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.section .text
.global _start
.type _start, @function

.code32
serial_print:
    push %eax
    push %ebx
    push %ecx
    push %edx

.next_char:
    movb (%esi), %al      # load byte from string
    testb %al, %al        # check for null terminator
    je .done_printing

.wait_thr:
    mov $0x3FD, %dx       # LSR of COM1
    inb %dx, %al          # read Line Status Register
    testb $0x20, %al      # is THR empty?
    jz .wait_thr

    movb (%esi), %al      # load byte again
    mov $0x3F8, %dx       # COM1 data port
    outb %al, %dx         # send character
    inc %esi               # move to next char
    jmp .next_char

.done_printing:
    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
    ret


_start:
    cli # begin by disabling interrupts
    mov $stack_top, %esp

    push %ebx # Save multiboot info

    movl $0xC001C0DE, magic_number

    leal test_msg, %esi
    call serial_print

    call check_multiboot
    call check_cpuid
    call check_long_mode

    pop %ebx # we need the multiboot info for next call
    push %ebx # we still want the info later
    call read_multiboot2

    call setup_paging
    

read_multiboot2:
    # save values in registers
    push %ebp
    mov %esp, %ebp
    push %ebx
    push %esi
    push %edi

    mov 8(%ebp), %esi  # Get multiboot info address from stack

    add $8, %esi  # Skip total_size (4) + reserved (4)
.tag_loop:
    mov (%esi), %eax      # load tag type
    cmp $0, %eax          # type 0 = end tag
    je .done_tags

    cmp $8, %eax          # framebuffer tag
    je .found_framebuffer

    cmp $6, %eax          # memory map tag
    je .found_mmap
.next_tag:
    mov 4(%esi), %edx  # tag size
    add %edx, %esi        # move to next tag
    add $7, %esi
    and $~7, %esi         # align to 8 bytes
    jmp .tag_loop
.found_framebuffer:
    mov 8(%esi), %eax           # framebuffer_addr low 32-bit
    movl %eax, framebuffer_address
    mov 12(%esi), %eax           # framebuffer_addr low 32-bit
    movl %eax, framebuffer_address+4
    mov 16(%esi), %eax          # pitch (bytes per row)
    mov %eax, pitch
    mov 20(%esi), %eax          # width
    mov %eax, screen_width
    mov 24(%esi), %eax          # height
    mov %eax, screen_height
    movzbl 28(%esi), %eax       # bits per pixel (zero-extend byte to 32-bit)
    mov %eax, bits_per_pixel
    mov framebuffer_address, %edi  # Load the ADDR into %edi
    movl $0x00FF0000, (%edi)       # Write RED to the first pixel
    mov framebuffer_address, %edi
    mov screen_width, %eax
    imul screen_height, %eax    # Total pixels = width * height
    mov %eax, %ecx              # Loop counter

    mov $0x00FF0000, %eax       # Color: Red (assuming 32bpp XRGB)

fill_loop:
    movl %eax, (%edi)           # Write color to current pixel
    add $4, %edi                # Move to next pixel (4 bytes for 32bpp)
    loop fill_loop              # Repeat until %ecx is 0
    jmp .next_tag
.found_mmap:
    mov 4(%esi), %ecx       # tag_size
    mov 8(%esi), %ebx       # entry_size
    lea 16(%esi), %edi      # pointer to first entry
    add %esi, %ecx            # ecx = end of tag

    # 0 out eax
    xor %eax, %eax
    mov %eax, total_physical_memory
    mov %eax, total_physical_memory+4
.mmap_loop:
    cmp %ecx, %edi
    jge .mmap_done

    mov 16(%edi), %eax # entry type
    cmp $1, %eax        # 1 = available RAM
    jne .mmap_next

    mov 8(%edi), %eax   # length low
    add %eax, total_physical_memory
    mov 12(%edi), %eax  # length high
    adc %eax, total_physical_memory+4
.mmap_next:
    add %ebx, %edi           # next entry
    jmp .mmap_loop
.mmap_done:
    jmp .next_tag
.done_tags:
    mov $'D', %al # 'F' for Framebuffer
    mov $0x3F8, %dx
    outb %al, %dx
    mov framebuffer_address, %eax 
    cmp $0, %eax
    je .no_framebuffer

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    ret
.no_framebuffer:
    leal no_framebuffer_msg, %esi
    call serial_print
    hlt

check_multiboot:
    cmp $0x36d76289, %eax # make sure we have magic number from multiboot
    jne .no_multiboot
    ret
.no_multiboot:
    leal no_multiboot_msg, %esi
    call serial_print
    hlt

check_cpuid:
    pushfl # save EFLAGS
    pushfl # store EFLAGS
    xorl $0x00200000, (%esp) # invert stored id bit
    popfl # load EFLAGS with inverted id bit
    pushfl # store EFlAGS again with maybe inverted bit
    pop %eax # eax = modified EFLAGS (ID bit may or may not be inverted)
    xorl (%esp), %eax # eax = whichever bits were changed
    popfl # Restore original EFLAGS
    testl $0x00200000, %eax # did id change?
    jz .no_cpuid # no change? -> thats not supported
    ret
.no_cpuid:
    leal no_cpuid_msg, %esi
    call serial_print
    hlt

check_long_mode:
    mov $0x80000000, %eax   # Request max extended CPUID leaf
    cpuid
    cmp $0x80000001, %eax   # Make sure CPU supports extended leaf 0x80000001
    jb .no_long_mode

    mov $0x80000001, %eax   # Request extended feature bits
    cpuid                    # CPUID now returns: EAX, EBX, ECX, EDX
    test $0x20000000, %edx   # Bit 29 in EDX = Long Mode
    jz .no_long_mode
    ret
.no_long_mode:
    leal no_long_mode_msg, %esi
    call serial_print
    hlt

setup_paging:
    
    mov $p4_table, %edi
    mov $4096*6, %ecx      # 1 P4 + 1 P3 + 4 P2 = 6 pages
    xor %eax, %eax
    rep stosb
    /* 1. Link P4 to P3 */
    mov $p3_table, %eax
    or $0x3, %eax
    mov %eax, p4_table

    mov $p2_table_1, %eax
    or $0x3, %eax
    mov %eax, p3_table

    mov $p2_table_2, %eax
    or $0x3, %eax
    mov %eax, p3_table + 8

    mov $p2_table_3, %eax
    or $0x3, %eax
    mov %eax, p3_table + 16

    mov $p2_table_4, %eax
    or $0x3, %eax
    mov %eax, p3_table + 24

    /* 3. Map P2 to Physical RAM (Huge Pages) */
    mov $0, %ecx
.code32
.map_p2_table_1:
    mov %ecx, %eax
    shl $21, %eax # 2MiB pages
    or $0b10000011, %eax # present + writable + huge page
    mov %eax, p2_table_1(,%ecx,8)
    movl $0, p2_table_1+ 4(,%ecx,8)
    inc %ecx
    cmp $512, %ecx
    jne .map_p2_table_1

    xor %ecx, %ecx
.map_p2_table_2:
    mov %ecx, %eax
    shl $21, %eax # 2MiB pages
    add $0x40000000, %eax
    or $0b10000011, %eax # present + writable + huge page
    mov %eax, p2_table_2(,%ecx,8)
    movl $0, p2_table_2+ 4(,%ecx,8)
    inc %ecx
    cmp $512, %ecx
    jne .map_p2_table_2

    xor %ecx, %ecx
.map_p2_table_3:
    mov %ecx, %eax
    shl $21, %eax # 2MiB pages
    add $0x80000000, %eax
    or $0b10000011, %eax # present + writable + huge page
    mov %eax, p2_table_3(,%ecx,8)
    movl $0, p2_table_3+ 4(,%ecx,8)
    inc %ecx
    cmp $512, %ecx
    jne .map_p2_table_3

    xor %ecx, %ecx
.map_p2_table_4:
    mov %ecx, %eax
    shl $21, %eax # 2MiB pages
    add $0xC0000000, %eax
    or $0b10000011, %eax # present + writable + huge page
    mov %eax, p2_table_4(,%ecx,8)
    movl $0, p2_table_4+ 4(,%ecx,8)
    inc %ecx
    cmp $512, %ecx
    jne .map_p2_table_4

    xor %ecx, %ecx

    mov $p4_table, %eax
    mov %eax, %cr3

    leal test_msg, %esi
    call serial_print

    mov %cr4, %eax
    or $0x20, %eax          # PAE
    mov %eax, %cr4

    leal test_msg, %esi
    call serial_print

    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax         # LME
    wrmsr

    leal test_msg, %esi
    call serial_print

    mov %cr0, %eax
    or $0x80000000, %eax    # PG
    mov %eax, %cr0

    leal test_msg, %esi
    call serial_print

    mov %cr0, %eax
    test $0x80000000, %eax   # check PG enabled
    jz .fail_pg

    mov %cr4, %eax
    test $0x20, %eax          # check PAE enabled
    jz .fail_pae

    rdmsr
    test $0x100, %eax         # check EFER.LME
    jz .fail_lme

    lgdt gdt64_ptr
    ljmp $0x08, $long_mode_start
.fail_pg:
    leal fail_pg_msg, %esi
    call serial_print
    hlt
.fail_pae:
    leal fail_pae_msg, %esi
    call serial_print
    hlt
.fail_lme:
    leal fail_lme_msg, %esi
    call serial_print
    hlt


/* SWITCH TO 64-BIT ASSEMBLY MODE */
.code64
long_mode_start:

    /* In 64-bit mode, segments are mostly ignored (null) */
    xor %eax, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    mov %eax, %ss

    mov $stack_top, %rsp

    /* Prepare arguments for kernel_main(magic, info_ptr)
       System V ABI: 1st arg in %rdi, 2nd in %rsi 
       Since we moved %eax/%ebx into %edi/%esi earlier, we are ready.
    */

    mov %rsp, %rdi
    pop %rsi

    call kernel_main

    cli
1:  hlt
    jmp 1b

/*
Set the size of the _start symbol to the current location '.' minus its start.
This is useful when debugging or when you implement call tracing.
*/
.size _start, . - _start

.section .rodata
gdt64:
    .quad 0                         # Null descriptor
    .quad (1<<43) | (1<<44) | (1<<47) | (1<<53)  # Code descriptor
.global gdt64_ptr
gdt64_ptr:
    .word 16-1     # limit = size of two descriptors minus 1
    .quad gdt64    # base