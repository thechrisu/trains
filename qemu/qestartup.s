/*.section IV, "x" /* Interrupt vector containing executable code */
.global _QeReset
_QeReset:
	LDR sp, =stack_top
	BL main
