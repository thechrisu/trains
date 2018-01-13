.text
  .code 32

  .global vec_start
  .global vec_end

swi_handler_addr: .word swi_handler
reset_handler_addr: .word reset_handler

vec_start:
  B reset_handler /* Reset */
  #LDR PC, reset_handler_addr
  B . /* Undefined */
  #LDR PC, swi_handler_addr
  B swi_handler /* SWI */
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
	LDR pc, =0xffffff
	#B . /* BRANCH */
