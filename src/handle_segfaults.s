.text
.global handle_segfault
.type	handle_segfault, %function
handle_segfault:
#    mov pc, #0x03000000
    sub r1, r14, #8
    ldr r0, =segfaultstr
    ldr r2, [r1, #0]
    BL bwprintf(PLT)
    B panic_exit

.section .rodata
segfaultstr: .ascii "\033[94m\033[5mSegmentation fault!\033[25m Address: 0x%x, Instruction 0x%x\033[39m\n\r\n"
