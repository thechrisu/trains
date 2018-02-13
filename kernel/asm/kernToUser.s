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
