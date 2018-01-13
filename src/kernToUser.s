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
/*
  Load address to trapframe into stack pointer. This address
  was passed as an argument to switch.
*/
  MOV sp, r1

  LDR r1, [sp, #4]

  mov r4, #0x01F00000
  add r4, r4, #0xC0000
  add r4, r4, #0xFF00
  add r4, r4, #0xCF
  cmp sp, r4
  beq crash


/* LOAD SPECIAL REGISTERS */
/* Save return value in r0 */
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
  ADD sp, #64

/* Go back to user mode (MOVS updates the PSR) */
/* Branch to instruction after SWI, giving control back to user task */
  MOVS pc, lr
