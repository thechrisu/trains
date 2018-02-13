.text
.global enter_kernel
.type	enter_kernel, %function
enter_kernel: /* called on an interrupt */
/* Check if we got here from a hardware interrupt. */
  CMP sp, #0xDA000000
  BEQ is_irq

/* Enter system mode. */
  MSR cpsr_c, #0xDF

/* Save user's registers in a trap frame on the user task's stack. */
  SUB sp, sp, #72
  STMEA sp, {r0-r14}

/* Set argument for handle_interrupt to user stack pointer. */
  MOV r0, sp

/* Set second argument to false. */
  MOV r1, #0

/* Enter kernel mode. */
  MSR cpsr_c, #0xD3

/* Put the kernel link register in the k_lr field of the trapframe on the user stack. */
  STR r14, [r0, #64]

  B is_swi

/* Enter system mode. */
is_irq:
  MSR cpsr_c, #0xDF

/* Save user's registers in a trap frame on the user task's stack. */
  SUB sp, sp, #72
  STMEA sp, {r0-r14}

/* Set argument for handle_interrupt to user stack pointer. */
  MOV r0, sp

/* Set second argument to true. */
  MOV r1, #1

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

/*
  Put a magic value in lr_irq so that we can assert we don't read lr_irq on software interrupt.
  Add 0x4 to it because, if it's read out later, it will be decreased by 4.
*/
  LDR lr, =0xA1B2C3D8

is_swi:

/* Put the saved program status register in the psr field. */
  MRS r4, spsr
  STR r4, [r0, #68]

/* Re-enter kernel mode (in case we were in IRQ mode). */
  MSR cpsr_c, #0xD3

/* Service interrupt. */
  BL handle_interrupt

/* Load kernel trapframe to return to instruction after e.g. task_activate in schedule. */
  LDMFD sp, {r0-r13}
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
  MSR cpsr_c, #0xDF
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
  MOV r2, sp
  MSR cpsr_c, #0xD7
  MOV r13, #0x01f00000
  MOV r0, #1
  MOV r1, r14
  B handle_abort

.text
.global handle_data_abort
.type handle_data_abort, %function
handle_data_abort:
  MSR cpsr_c, #0xDF
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
  MOV r2, sp
  MSR cpsr_c, #0xD7
  MOV r13, #0x01f00000
  MOV r0, #2
  MOV r1, r14
  B handle_abort
