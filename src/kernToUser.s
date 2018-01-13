# TODO store/load multiple
# TODO really get register values
# TODO test
# TODO write wrapper for this
switch: # switches from one task to another one. Does r0 include sp?
/* We don't save r0-r3, they're arguments */
  SUB sp, =60
  STR r4, sp, #16
  STR r5, sp, #20
  STR r6, sp, #24
  STR r7, sp, #28
  STR r8, sp, #32
  STR r9, sp, #36
  STR r10, sp, #40
  STR r11, sp, #44
  STR r12, sp, #48
  STR r13, sp, #52
  STR r14, sp, #56
  STR r15, sp, #60

/* SAVE SPECIAL REGISTERS */
  STR fp, sp, #0
  STR ir, sp, #4
  STR lr, sp, #8

/* STACK POINTER */
  MOV r0, r1 # move r0 to the address pointed to by the 'new' sp (saved in r1)
  LDR sp, r1

/* LOAD SPECIAL REGISTERS */
/* Save return value in r0 */
  LDR r0, sp, #0
  LDR r1, sp, #4
  LDR r2, sp, #8
  LDR r3, sp, #12
  LDR r4, sp, #16
  LDR r5, sp, #20
  LDR r6, sp, #24
  LDR r7, sp, #28
  LDR r8, sp, #32
  LDR r9, sp, #36
  LDR r10, sp, #40
  LDR r11, sp, #44
  LDR r12, sp, #48
  LDR r13, sp, #52
  LDR r14, sp, #56
  LDR r15, sp, #60
  ADD sp, =60