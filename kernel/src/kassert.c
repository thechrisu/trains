#include "kassert.h"
#include "syscall/syscall.h"

void __kassert(bool value, const char *expression, const char *caller_name, const char *file_name, int line_num) {
  if (unlikely(!value)) {
#ifndef TESTING
    dump_logs();
    log_index = 0;

    bwprintf("\033[31mAssertion failed! \"%s\" in function \"%s\" at %s:%d\033[39m\n\r", expression, caller_name, file_name, line_num);
#endif /* TESTING */
    syscall_panic();
  }
}
