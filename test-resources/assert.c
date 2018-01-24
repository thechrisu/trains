#include "assert.h"

#ifndef TESTING
void __assert(bool expr, const char * caller_name, const char *file_name, int line_num) {
#if DEBUG
  if (!expr) {
    printf("\033[4;1HAssertion failed! \"%s\" in %s:%d\n", caller_name, file_name, line_num);
  }
#else
  (void)expr;
  (void)caller_name;
  (void)file_name;
  (void)line_num;
#endif /* DEBUG */
}
#endif /* TESTING */
