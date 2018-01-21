.text
.global handle_abort
.type	handle_abort, %function
handle_abort:
    sub r1, r14, #8
    ldr r0, =abortstr
    ldr r2, [r1, #0]
    BL bwprintf(PLT)
    B panic_exit

.section .rodata
abortstr: .ascii "\033[94m\033[5mSegmentation fault!\033[25m Address: 0x%x, Instruction 0x%x\033[39m\n\r\n"
