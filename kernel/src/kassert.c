#include "kassert.h"
#include "syscall/syscall.h"

void __kassert(bool value, const char * caller_name, const char *file_name, int line_num) {
  if (unlikely(!value)) {
    dump_logs();
    log_index = 0;

#ifndef TESTING
    bwprintf("\033[31mAssertion failed! \"%s\" at %s:%d\033[39m\n", caller_name, file_name, line_num);
#endif /* TESTING */
    syscall_panic();
  }
}
