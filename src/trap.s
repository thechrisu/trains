.text
.global enter_kernel
.type	enter_kernel, %function
enter_kernel: /* called on an interrupt */
/* Save user's registers in a trap frame on the user task's stack */
  #CMP sp, #64
  #BEQ crash
  //B crash


  MSR cpsr_c, 0x1F

  SUB sp, #68
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

  MSR cpsr_c, 0x13
  STR r14, [r0, #4]

/* Change sp to kernel sp */
/* Load global offset table base address into r4 */
	LDR	r4, global_offset_table_information
need_kernel_sp:
/*
  Add pc (which will be equal to need_kernel_sp + 8 when
  it is actually read for the ADD instruction) to r4. Once
  we reach need_kernel_sp + 8, r4 will just contain
  _GLOBAL_OFFSET_TABLE_.
*/
	ADD	r4, pc, r4
/* Load offset of kernel_stack_pointer within global offset table. */
	LDR	r3, global_offset_table_information + 4
/* Update stack pointer. */
	LDR	sp, [r4, r3]

/* Service interrupt */
  B handle_interrupt

global_offset_table_information:
	.word	_GLOBAL_OFFSET_TABLE_ - (need_kernel_sp + 8)
	.word	kernel_stack_pointer(GOT)
