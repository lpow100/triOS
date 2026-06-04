.global setjmp
.global longjmp

.text

# -----------------------------------------------------------------------------
# int setjmp(jmp_buf env)
# %rdi = pointer to our 8-element uint64_t array
# -----------------------------------------------------------------------------
setjmp:
    # 1. Save all standard callee-saved registers into the buffer
    movq %rbx, 0(%rdi)
    movq %rbp, 8(%rdi)
    movq %r12, 16(%rdi)
    movq %r13, 24(%rdi)
    movq %r14, 32(%rdi)
    movq %r15, 40(%rdi)

    # 2. Save the Stack Pointer (RSP) *as it was before this call instruction*
    # When 'call setjmp' executed, it pushed an 8-byte return address onto the stack.
    # Therefore, the true caller stack frame pointer is RSP + 8.
    leaq 8(%rsp), %rax
    movq %rax, 48(%rdi)

    # 3. Save the Instruction Pointer (RIP)
    # The return address is sitting right at the top of the current stack frame.
    movq (%rsp), %rax
    movq %rax, 56(%rdi)

    # 4. Return 0 for the initial direct execution setup pass
    xorl %eax, %eax
    ret

# -----------------------------------------------------------------------------
# void longjmp(jmp_buf env, int val)
# %rdi = pointer to saved jmp_buf array
# %rsi = integer value to return instead of 0
# -----------------------------------------------------------------------------
longjmp:
    # 1. Move our desired return value (%rsi) into %eax. 
    # POSIX says if val (%rsi) is 0, longjmp must force a return value of 1.
    movl %esi, %eax
    testl %eax, %eax
    jnz .skip_zero_force
    movl $1, %eax
.skip_zero_force:

    # 2. Restore all preserved registers back from the buffer snapshot
    movq 0(%rdi), %rbx
    movq 8(%rdi), %rbp
    movq 16(%rdi), %r12
    movq 24(%rdi), %r13
    movq 32(%rdi), %r14
    movq 40(%rdi), %r15

    # 3. Warp the Stack Pointer back to the caller frame environment
    movq 48(%rdi), %rsp

    # 4. Grab the saved return instruction address (RIP) 
    movq 56(%rdi), %rdx

    # 5. Recreate the stack layout expected by a 'ret' instruction:
    # Push the destination execution address onto our newly restored stack frame.
    pushq %rdx

    # 6. Jump! This pops the pushed RIP and resumes code right after the original setjmp call.
    ret