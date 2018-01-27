.text
.global enter_kernel
.type	enter_kernel, %function
enter_kernel: /* called on an interrupt */
/* Enter system mode. */
  MSR cpsr_c, #0xDF

/* Save user's registers in a trap frame on the user task's stack. */
  SUB sp, sp, #72
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

/* Set argument for handle_interrupt to user stack pointer. */
  MOV r0, sp

/* Enter kernel mode. */
  MSR cpsr_c, #0xD3

/* Put the kernel link register in the k_lr field of the trapframe on the user stack. */
  STR r14, [r0, #64]

/* Put the saved program status register in the psr field. */
  MRS r4, spsr
  STR r4, [r0, #68]

/* Tell handle_interrupt that we're calling it because of a software interrupt. */
  MOV r1, #0x0

/* Service interrupt. */
  BL handle_interrupt

/* Load kernel trapframe to return to instruction after e.g. task_activate in schedule. */
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
  ADD sp, sp, #72
  LDR r15, [sp, #-16] /* Update me if sizeof(trapframe) changes */

.text
.global enter_kernel_irq
.type	enter_kernel_irq, %function
enter_kernel_irq: /* called on an interrupt */
/* Enter system mode. */
  MSR cpsr_c, #0xDF

/* Save user's registers in a trap frame on the user task's stack. */
  SUB sp, sp, #72
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

/* Set argument for handle_interrupt to user stack pointer. */
  MOV r0, sp

/* Enter IRQ mode. */
  MSR cpsr_c, #0xD2

/*
  Due to pipelining, the IRQ link register points to the instruction after the
  instruction that we want to return to in the user task. Subtract four to account
  for this discrepancy.
*/
  SUB r14, r14, #4

/* Put the IRQ link register in the k_lr field of the trapframe on the user stack. */
  STR r14, [r0, #64]

/* Put the saved program status register in the psr field. */
  MRS r4, spsr
  STR r4, [r0, #68]

/* Enter kernel mode. */
  MSR cpsr_c, #0xD3

/* Tell handle_interrupt that we're calling it because of a hardware interrupt. */
  MOV r1, #0x1

/* Service interrupt. */
  BL handle_interrupt

/* Load kernel trapframe to return to instruction after e.g. task_activate in schedule. */
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
  ADD sp, sp, #72
  LDR r15, [sp, #-16] /* Update me if sizeof(trapframe) changes */

.text
.global sys_exit
.type	sys_exit, %function
sys_exit:
  mov r1, r0
  /* SYS_EXIT in syscall/codes.h */
  mov r0, #0
  swi 0

.text
.global handle_undefined_abort
.type handle_undefined_abort, %function
handle_undefined_abort:
  MOV r13, #0x01f00000
  MOV r0, #0
  MOV r1, r14
  B handle_abort

.text
.global handle_prefetch_abort
.type handle_prefetch_abort, %function
handle_prefetch_abort:
  MOV r13, #0x01f00000
  MOV r0, #1
  MOV r1, r14
  B handle_abort

.text
.global handle_data_abort
.type handle_data_abort, %function
handle_data_abort:
  MOV r13, #0x01f00000
  MOV r0, #2
  MOV r1, r14
  B handle_abort
