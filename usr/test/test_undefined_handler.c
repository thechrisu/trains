#include "test_undefined_handler.h"
#include "../include/common/codes.h"
#include "user_data_abort.h"
#include "myio.h"

void test() {

}

void test_undefined_handler() {
  bwprintf("");
  UNDEFINED();
  bwprintf("FirstUserTask: exiting\n\r");
  Exit();
}
