#include "test_runner.h"

void exec_prog(int priority, void (*code)()) {
  Create(priority, code);
  bwprintf("ENDPROG\n\r");
}

void test_runner() {
  char c = '\0';
  char buf[512];
  int i;

  do {
    i = 0;
    buf[0] = '\0';

    bwprintf("Enter a test program (q to exit): \n\r");

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
      bwprintf("ENDPROG\n\r");
    } else if (strcmp(buf, "messaging_basic")) {
      exec_prog(5, &test_messaging_basic);
    } else if (strcmp(buf, "messaging_receive_before_send")) {
      exec_prog(5, &test_messaging_receive_before_send);
    } else if (strcmp(buf, "messaging_sequence")) {
      exec_prog(5, &test_messaging_sequence);
    } else if (strcmp(buf, "messaging_truncation")) {
      exec_prog(5, &test_messaging_truncation);
    } else if (strcmp(buf, "messaging_invalid_tid")) {
      exec_prog(5, &test_messaging_invalid_tid);
    } else if (strcmp(buf, "messaging_fifo_send")) {
      exec_prog(5, &test_messaging_fifo_send);
    } else if (strcmp(buf, "messaging_same_priority")) {
      exec_prog(5, &test_messaging_same_priority);
    } else if (strcmp(buf, "messaging_send_recipient_zombie")) {
      exec_prog(5, &test_messaging_send_recipient_zombie);
    } else if (strcmp(buf, "messaging_reply_target_zombie")) {
      exec_prog(5, &test_messaging_reply_target_zombie);
    } else if (strcmp(buf, "messaging_tree")) {
      exec_prog(5, &test_messaging_tree);
    } else if (strcmp(buf, "messaging_exit_with_blocked")) {
      exec_prog(5, &test_messaging_exit_with_blocked);
    } else if (strcmp(buf, "test_nameserver_errors")) {
      exec_prog(5, &test_nameserver_errors);
    } else if (strcmp(buf, "test_nameserver_too_many")) {
      exec_prog(5, &test_nameserver_too_many);
    } else if (strcmp(buf, "test_nameserver_happypath")) {
      exec_prog(5, &test_nameserver_happypath);
    } else if (strcmp(buf, "test_undefined_handler")) {
      exec_prog(5, &test_undefined_handler);
    } else if (!strcmp(buf, "q")) {
      bwprintf("Unknown test program \"%s\"\n\r", buf);
    }
  } while (!strcmp(buf, "q"));
}
