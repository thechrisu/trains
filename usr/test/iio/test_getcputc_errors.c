#include "test_getcputc_errors.h"

void test_getcputc_errors() {
  // servers not yet up
  bwprintf("Servers not yet up, Getc %d == -1", Getc(-1, TRAIN));
  bwprintf("Servers not yet up, Putc %d == -1", Putc(-1, TRAIN, 'c'));
}
