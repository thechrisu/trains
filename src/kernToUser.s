/*
TODO store/load multiple
TODO really get register values
TODO test
TODO write wrapper for this
*/
.text
.global leave_kernel
.type	leave_kernel, %function
leave_kernel:
  STR r14, [sp, #0]
  SUB sp, sp, #4
/*
  Set saved PSR mode so that, when we call movs, we'll go into usermode.
*/
  MSR SPSR_c, #0x10

/*
  Store kernel link register in trapframe's k_lr member variable.
*/
  LDR r14, [r1, #64]

/*
  Go into system mode.
*/
  MSR cpsr_c, #0x1F

/*
  Load address to trapframe into stack pointer. This address
  was passed as an argument to switch.
*/
  MOV sp, r1

/*
  Load user registers from trapframe. Syscall return value is saved in r0.
*/
  LDR r1, [sp, #4]
  LDR r2, [sp, #8]
  LDR r3, [sp, #12]
  LDR r4, [sp, #16]
  LDR r5, [sp, #20]
  LDR r6, [sp, #24]
  LDR r7, [sp, #28]
  LDR r8, [sp, #32]
  LDR r9, [sp, #36]
  LDR r10, [sp, #40]
  LDR r11, [sp, #44]
  LDR r12, [sp, #48]
  LDR r13, [sp, #52]
  LDR r14, [sp, #56]
  ADD sp, sp, #68

/*
  Back to kernel mode.
*/
  MSR cpsr_c, #0x13

/*
  Go back to user mode (MOVS updates the PSR). Branch to instruction after SWI,
  giving control back to user task.
*/
  MOVS pc, lr
