.text
  .code 32

  .global vec_start
  .global vec_end

vec_start:
  B reset_handler /* Reset */
  B . /* Undefined */
  B enter_kernel /* SWI */
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
