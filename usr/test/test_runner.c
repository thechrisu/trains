#include "test_runner.h"

void exec_prog(int priority, void (*code)()) {
  Create(priority, code);
  bwprintf("ENDPROG\r");
}

void test_runner() {
  char c = '\0';
  char buf[512];
  int i;

  do {
    i = 0;
    buf[0] = '\0';

    bwprintf("Enter a test program (q to exit): \r");

    c = bwgetc(TERMINAL);
    while (c != '\r') {
      buf[i] = c;
      i += 1;
      putc(TERMINAL, c);
      c = bwgetc(TERMINAL);
    }

    bwprintf("\n\r");

    buf[i] = '\0';

    if (strcmp(buf, "a0")) {
      exec_prog(2, &a0_main);
    } else if (strcmp(buf, "k1")) {
      exec_prog(5, &k1_first_user_task);
    } else if (strcmp(buf, "test")) {
      bwprintf("TestReturn\n\r", buf);
      bwprintf("ENDPROG\r");
    } else if (strcmp(buf, "messaging_basic")) {
      exec_prog(5, &test_messaging_basic);
    } else if (strcmp(buf, "messaging_receive_before_send")) {
      exec_prog(5, &test_messaging_receive_before_send);
    } else if (strcmp(buf, "messaging_sequence")) {
      exec_prog(5, &test_messaging_sequence);
    } else if (!strcmp(buf, "q")) {
      bwprintf("Unknown test program \"%s\"\n\r", buf);
    }
  } while (!strcmp(buf, "q"));
}
