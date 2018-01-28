.text
  .code 32

  .global vec_start
  .global vec_end

vec_start:
  B reset_handler /* Reset */
  B handle_undefined_abort /* Undefined */
  B enter_kernel /* SWI */
  B handle_prefetch_abort /* Prefetch Abort */
  B handle_data_abort /* Data Abort */
  B . /* reserved */
  B . /* IRQ */
  B . /* FIQ */

vec_end:

reset_handler:
	LDR sp, =stack_top
	BL cp_vectors
	BL main
