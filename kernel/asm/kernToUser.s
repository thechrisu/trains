/*
TODO store/load multiple
*/
.text
.global leave_kernel
.type	leave_kernel, %function
leave_kernel:
  /* Store kernel trapframe. */
  SUB sp, sp, #72
  STMEA sp, {r0-r14}
  /*
  STR r0, [sp, #0]
  STR r1, [sp, #4]
  STR r2, [sp, #8]
  STR r3, [sp, #12]
  STR r4, [sp, #16]
  STR r5, [sp, #20]
  STR r6, [sp, #24]
  STR r7, [sp, #28]
  STR r8, [sp, #32]
  STR r9, [sp, #36]
  STR r10, [sp, #40]
  STR r11, [sp, #44]
  STR r12, [sp, #48]
  STR r13, [sp, #52]
  STR r14, [sp, #56]
  */

/* Put the trapframe's psr field in the saved program status register. */
  LDR r4, [r1, #68]
  MSR spsr, r4

/*
  Load kernel link register from trapframe's k_lr member variable.
*/
  LDR r14, [r1, #64]

/*
  Go into system mode.
*/
  MSR cpsr_c, #0xDF

/*
  Load address to trapframe into stack pointer. This address
  was passed as an argument to leave_kernel.
*/
  MOV sp, r1

/*
  Load user registers from trapframe. Syscall return value is saved in r0.
  */
  LDMFD sp, {r0-r14}

  /*
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
  */
  ADD sp, sp, #72

/*
  Back to kernel mode.
*/
  MSR cpsr_c, #0xD3

/*
  Go back to user mode (MOVS updates the PSR). Branch to instruction after SWI,
  giving control back to user task.
*/
  MOVS pc, lr
