.text
  .code 32

  .global vec_start
  .global vec_end

vec_start:
  B reset_handler /* Reset */
  B . /* Undefined */
  B .
  B . /* Prefetch Abort */
  B . /* Data Abort */
  B . /* reserved */
  B . /* IRQ */
  B . /* FIQ */

vec_end:

reset_handler:
	LDR sp, =stack_top
	BL cp_vectors
	BL main
	MOV r0, #0x18
	MOV r1, #0x20000
	ADD r1, r1, #0x026
    SVC 0x00123456
