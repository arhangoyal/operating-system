.section .data
.global sig_flag
sig_flag:
    .long 0    # Initialize the flag to 0


.section .text
.global sys_calls_handler
.global sys_calls_handler_end
.global halt_return
.global return_to_parent

/*
 * System Call Dispatcher
 * Description:
 *    This function is the entry point for all system calls in the kernel. It dispatches
 *    the system call to the appropriate handler based on the system call number passed
 *    in the %eax register. The function saves callee-saved registers (%ebx, %esi, %edi),
 *    checks the validity of the system call number, and then jumps to the corresponding
 *    system call handler using a jump table. If the system call number is invalid, it
 *    returns an error.
 * Inputs:
 *    %eax - System call number.
 *    %ebx, %ecx, %edx - Arguments for the system call, depending on the call.
 * Outputs:
 *    %eax - Return value from the system call. -1 if the system call number is invalid.
 * Side Effects:
 *    Modifies stack to save callee-saved registers and potentially changes processor state
 *    and memory based on the system call executed.
 */
sys_calls_handler:
    pushl   %ebp /* Save base pointer */
    movl    %esp, %ebp /* Set base pointer to current stack pointer */

    pushl   %ebx 
    pushl   %esi 
    pushl   %edi /* Save callee saved registers */

    
    pushl %ecx
    pushl %edx
    pushl %eax
    call update_user_esp_syscall
    popl %eax
    popl %edx
    popl %ecx

    cmpl    $1, %eax
    jl      return_error /* If call number < 1, error */
    cmpl    $10, %eax
    jg      return_error /* If call number > 10, error */

    pushl   %edx /* Push system call arguments onto the stack */
    pushl   %ecx
    pushl   %ebx

    pushl $0 /* Maintain c calling convention stack spacing for function args */

    jmp     *jump_table(, %eax, 4) /* Jump to the appropriate syscall */

return_error:
    movl    $-1, %eax  /* return with error value */
    subl    $20, %esp /* Adjust stack pointer to pop arguments */
    jmp     sys_calls_handler_end /* Jump to end of system call handler to restore state and return */

sys_calls_handler_end:
    addl    $20, %esp             /* pop arguments */

    /* Check if sig_flag is 1 */
    movl    sig_flag, %ecx        /* Load the value of sig_flag into eax */
    cmpl    $1, %ecx              /* Compare sig_flag with 1 */
    je      skip_call             /* Jump to skip_call if sig_flag is equal to 1 */

    pushl   %eax                  /* Push eax to preserve it */
    call    sig_test              /* Call sig_test */
    popl    %eax                  /* Restore eax */

skip_call:
    movl    $0, sig_flag          /* Set sig_flag to 0 regardless of previous value */

    popl    %ebx 
    popl    %esi 
    popl    %edi /* Restore callee saved registers */

    popl %ebp
    ret /* Return from system call */

jump_table:
        .long 0x1, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

/* define halt_return(parent_esp, parent_ebp, ret_val) */
halt_return:
    movl 12(%esp), %eax /* Load return value into %eax */
    movl 8(%esp), %ebp /* Restore parent's base pointer */

    movl %ebp, %esp /* Set stack pointer to base pointer, effectively restoring parent's stack frame */
    jmp sys_calls_handler_end /* Jump to the end of system call handler to restore registers and return */
    
return_to_parent:
    movl 4(%esp), %ebp /* Restore parent's base pointer */
    movl %ebp, %esp /* Set stack pointer to base pointer, effectively restoring parent's stack frame */
    popl %ebp /* Restore the previous EBP */
    ret /* Jump to the end of system call handler to restore registers and return */
