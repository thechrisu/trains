#include "assert.h"

extern int handle_interrupt_fp;
extern int handle_interrupt_sp;

void __kassert(bool value, const char * caller_name, const char *file_name, int line_num) {
  if (unlikely(!value)) {
    bwprintf("\033[31mAssertion failed! \"%s\" at %s:%d\033[39m\n", caller_name, file_name, line_num);
    __asm__(
      "mov fp, %0\n\t"
      "mov sp, %1\n\t"
      "b kassert_exit"
    : : "r" (handle_interrupt_fp), "r" (handle_interrupt_sp));
  }
}
