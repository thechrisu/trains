#include "test_runner.h"

void test_runner() {
  char c = '\0';
  char buf[512];
  int i;

  do {
    i = 0;
    buf[0] = '\0';

    bwprintf("Enter a test program (q to exit): ");

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
      Create(2, &a0_main);
    } else if (strcmp(buf, "k1")) {
      Create(2, &k1_first_user_task);
    } else if (!strcmp(buf, "q")) {
      bwprintf("Unknown test program \"%s\"\n\r", buf);
    }
  } while (!strcmp(buf, "q"));
}
