.section IV, "x" /* Interrupt vector containing executable code */
.global _Reset

_Reset:
  B Reset_Handler /* Reset */
  B . /* Undefined */
  B . /* SWI */
  B . /* Prefetch Abort */
  B . /* Data Abort */
  B . /* reserved */
  B . /* IRQ */
  B . /* FIQ */

Reset_Handler:
	LDR sp, =stack_top
	BL main
	B . /* BRANCH */
