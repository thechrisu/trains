ENTRY (vec_start)
SECTIONS
{
  . = 0x10000;
  .text : {
  build/versatilepb/kernel/asm/startup.o
  *(.text)
  *(.rodata)
  }
  .data : { *(.data) }
  .bss : { *(.bss COMMON) }
  . = ALIGN(8);
  . = . + 0x1000;
  stack_top = .;
}
