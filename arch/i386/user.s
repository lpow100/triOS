.global jump_to_user_mode
jump_to_user_mode:
    # Disable interrupts before modifying segments to avoid a race condition
    cli

    # C Calling Convention (64-bit):
    # %rdi = Argument 1: Target entry point address (User RIP)
    # %rsi = Argument 2: Target User Stack pointer (User RSP)

    # Set up the data segments for Ring 3.
    # In 64-bit mode, ds, es, and ss are generally ignored by the CPU hardware,
    # but they should be set to 0 or a valid flat User Data selector (0x23) for safety.
    movw $0x23, %ax
    movw %ax, %ds
    movw %ax, %es

    # Line up the 64-bit stack parameters for the 'iretq' instruction.
    # The CPU expects exactly 5 quadwords (8 bytes each) on the stack:
    # [SS] -> [RSP] -> [RFLAGS] -> [CS] -> [RIP]
    
    pushq $0x23         # SS (User Data Segment Selector)
    pushq %rsi          # RSP (User Stack Pointer, from 2nd argument)
    
    pushfq              # Push current 64-bit RFLAGS
    popq %rcx           # Pop into a 64-bit scratch register
    orq $0x200, %rcx    # Set the IF bit (Bit 9) so interrupts are re-enabled in User Mode
    pushq %rcx          # Push modified RFLAGS back
    
    pushq $0x1B         # CS (User Code Segment Selector, 0x18 | 3 RPL)
    pushq %rdi          # RIP (Target Entry Point, from 1st argument)

    # Clear FS and GS or swap them if your OS utilizes swapgs
    # (Leaving this basic, but keep SWAPGS in mind if implementing full isolation)

    # Execute the 64-bit atomic stack pop!
    iretq