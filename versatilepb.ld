ENTRY (Reset_Handler)
SECTIONS
{
  . = 0x10000;
/*  .startup . : { build/versatilepb/src/startup.o(.text )} */
  .text : {
  *(.text)
  *(.rodata)
  }
  .data : { *(.data) }
  .bss : { *(.bss COMMON) }
  . = ALIGN(8);
  . = . + 0x1000;
  stack_top = .;
}
