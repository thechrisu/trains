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


    if (tstrcmp(buf, "a0")) {
      exec_prog(MyPriority() + 1, &a0_main);
    } else if (tstrcmp(buf, "k1")) {
      exec_prog(MyPriority() + 4, &k1_first_user_task);
    } else if (tstrcmp(buf, "k2")) {
      exec_prog(MyPriority() + 10, &k2_first_user_task);
    } else if (tstrcmp(buf, "test")) {
      bwprintf("TestReturn\n\r", buf);
      bwprintf("ENDPROG\n\r");
    } else if (tstrcmp(buf, "messaging_basic")) {
      exec_prog(MyPriority() + 4, &test_messaging_basic);
    } else if (tstrcmp(buf, "messaging_receive_before_send")) {
      exec_prog(MyPriority() + 4, &test_messaging_receive_before_send);
    } else if (tstrcmp(buf, "messaging_sequence")) {
      exec_prog(MyPriority() + 4, &test_messaging_sequence);
    } else if (tstrcmp(buf, "messaging_truncation")) {
      exec_prog(MyPriority() + 4, &test_messaging_truncation);
    } else if (tstrcmp(buf, "messaging_invalid_tid")) {
      exec_prog(MyPriority() + 4, &test_messaging_invalid_tid);
    } else if (tstrcmp(buf, "messaging_fifo_send")) {
      exec_prog(MyPriority() + 4, &test_messaging_fifo_send);
    } else if (tstrcmp(buf, "messaging_same_priority")) {
      exec_prog(MyPriority() + 4, &test_messaging_same_priority);
    } else if (tstrcmp(buf, "messaging_send_recipient_zombie")) {
      exec_prog(MyPriority() + 4, &test_messaging_send_recipient_zombie);
    } else if (tstrcmp(buf, "messaging_reply_target_zombie")) {
      exec_prog(MyPriority() + 4, &test_messaging_reply_target_zombie);
    } else if (tstrcmp(buf, "messaging_tree")) {
      exec_prog(MyPriority() + 4, &test_messaging_tree);
    } else if (tstrcmp(buf, "messaging_exit_with_blocked")) {
      exec_prog(MyPriority() + 4, &test_messaging_exit_with_blocked);
    } else if (tstrcmp(buf, "test_nameserver_errors")) {
      exec_prog(MyPriority() + 4, &test_nameserver_errors);
    } else if (tstrcmp(buf, "test_nameserver_wrapper_errors")) {
      exec_prog(MyPriority() + 4, &test_nameserver_wrapper_errors);
    } else if (tstrcmp(buf, "test_nameserver_too_many")) {
      exec_prog(MyPriority() + 4, &test_nameserver_too_many);
    } else if (tstrcmp(buf, "test_nameserver_happypath")) {
      exec_prog(MyPriority() + 4, &test_nameserver_happypath);
    } else if (tstrcmp(buf, "test_undefined_handler")) {
      exec_prog(MyPriority() + 4, &test_undefined_handler);
    } else if (tstrcmp(buf, "message_benchmark")) {
      exec_prog(MyPriority() + 6, &message_benchmark);
    } else if (tstrcmp(buf, "test_mypriority")) {
      exec_prog(MyPriority() + 4, &test_mypriority);
    } else if (!tstrcmp(buf, "q")) {
      bwprintf("Unknown test program \"%s\"\n\r", buf);
    }
  } while (!tstrcmp(buf, "q"));
}
