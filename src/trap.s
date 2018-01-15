.text
.global enter_kernel
.type	enter_kernel, %function
enter_kernel: /* called on an interrupt */
/* Enter system mode. */
  MSR cpsr_c, #0x1F

/* Save user's registers in a trap frame on the user task's stack. */
  SUB sp, sp, #68
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
  MSR cpsr_c, #0x13

/* Put the kernel link register in the k_lr field of the trapframe on the user stack. */
  STR r14, [r0, #64]
  LDR r14, [sp, #4]
/* Service interrupt. */
  B handle_interrupt

.text
.global sys_exit
.type	sys_exit, %function
sys_exit:
  mov r1, r0
  mov r0, #0
  swi 0
