/*.section IV, "x" /* Interrupt vector containing executable code */
.global _Reset
_Reset:
	LDR sp, =stack_top
	BL main
	B . /* BRANCH */
