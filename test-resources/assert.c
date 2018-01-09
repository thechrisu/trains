#include "assert.h"

void __assert(bool expr, const char * caller_name, const char *file_name, int line_num) {
  if (!expr) {
    printf("\033[4;1HAssertion failed! \"%s\" in %s:%d\n", caller_name, file_name, line_num);
  }
}
