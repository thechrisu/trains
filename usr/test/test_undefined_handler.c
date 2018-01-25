#include "test_undefined_handler.h"
#include "codes.h"
#include "user_data_abort.h"
#include "myio.h"

void test_undefined_handler() {
#pragma GCC diagnostic ignored "-Wformat-zero-length"
  bwprintf("");
#pragma GCC diagnostic warning "-Wformat-zero-length"
  UNDEFINED();
  bwprintf("FirstUserTask: exiting\n\r");
  Exit();
}
