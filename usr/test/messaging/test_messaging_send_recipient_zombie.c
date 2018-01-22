#include "test_messaging.h"

void zombie_recipient() {
  bwprintf("Exiting...\n\r");
}

void test_messaging_send_recipient_zombie() {
  int receiver_tid = Create(6, &zombie_recipient);
  int error_code = Send(receiver_tid, (char *)0, 0, (char *)0, 0);
  bwprintf("Error code: %d\n\r", error_code);
}
