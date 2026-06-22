# _entry.S (AT&T Syntax)
.global _start
.extern main

.text
_start:
    # 1. Call your shell's main C function
    call main

    # 2. Trigger the Ring 0 software interrupt exception (SYS_EXIT)
    movq $1, %rax       # System call ID 1
    movq $0, %rdi       # Return status code 0 (Success)
    int $0x80           # Software interrupt vector 0x80

    # 3. Ultimate hardware fallback protection loop
1:  hlt
    jmp 1b