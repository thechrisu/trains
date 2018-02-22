#include "test_runner.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
  char *name;
  void (*code)();
  int prio;
} test;

void print_help() {
  bwprintf("\n\r");
  bwprintf("%sHelp%s\n\r", BOLD_TEXT, RESET_TEXT);
  bwprintf("help\tPrint this help screen.\n\r");
  bwprintf("list\tList available tests.\n\r");
  bwprintf("\n\r");
}

void print_tests(test *tests) {
  bwprintf("\n\r");
  bwprintf("%sTests%s\n\r", BOLD_TEXT, RESET_TEXT);
  bwprintf("test\n\r");

  test *current_test = (test *)tests;
  while (!tstrcmp(current_test->name, "")) {
    bwprintf("%s\n\r", current_test->name);
    current_test += 1;
  }

  bwprintf("\n\r");
}

void run_test(test *tests, char *name) {
  if (tstrcmp(name, "help")) {
    print_help();
    return;
  } else if (tstrcmp(name, "list")) {
    print_tests(tests);
    return;
  } else if (tstrcmp(name, "test")) {
    bwprintf("TestReturn\n\r");
    return;
  }

  test *current_test = (test *)tests;
  while (!tstrcmp(current_test->name, "")) {
    if (tstrcmp(name, current_test->name)) {
      Create(MyPriority() + current_test->prio, current_test->code);
      return;
    }
    current_test += 1;
  }

  if (!tstrcmp(name, "q")) {
    bwprintf("Unknown test program \"%s\"\n\r", name);
  }
}

void exec_prog(int priority, void (*code)()) {
  Create(priority, code);
}

void test_runner() {
  char c = '\0';
  char buf[512];
  int i;

  test tests[] = {
    {
      .name = "k1",
      .code = &k1_first_user_task,
      .prio = 4
    },
    {
      .name = "k2",
      .code = &k2_first_user_task,
      .prio = 10
    },
    {
      .name = "k3",
      .code = &k3_first_user_task,
      .prio = 15
    },
    {
      .name = "k4",
      .code = &k4_first_user_task,
      .prio = 15
    },
    {
      .name = "messaging_basic",
      .code = &test_messaging_basic,
      .prio = 4
    },
    {
      .name = "messaging_receive_before_send",
      .code = &test_messaging_receive_before_send,
      .prio = 4
    },
    {
      .name = "messaging_sequence",
      .code = &test_messaging_sequence,
      .prio = 4
    },
    {
      .name = "messaging_truncation",
      .code = &test_messaging_truncation,
      .prio = 4
    },
    {
      .name = "messaging_invalid_tid",
      .code = &test_messaging_invalid_tid,
      .prio = 4
    },
    {
      .name = "messaging_fifo_send",
      .code = &test_messaging_fifo_send,
      .prio = 4
    },
    {
      .name = "messaging_same_priority",
      .code = &test_messaging_same_priority,
      .prio = 4
    },
    {
      .name = "messaging_send_recipient_zombie",
      .code = &test_messaging_send_recipient_zombie,
      .prio = 4
    },
    {
      .name = "messaging_reply_target_zombie",
      .code = &test_messaging_reply_target_zombie,
      .prio = 4
    },
    {
      .name = "messaging_tree",
      .code = &test_messaging_tree,
      .prio = 4
    },
    {
      .name = "messaging_exit_with_blocked",
      .code = &test_messaging_exit_with_blocked,
      .prio = 4
    },
    {
      .name = "test_nameserver_wrapper_errors",
      .code = &test_nameserver_wrapper_errors,
      .prio = 4
    },
    {
      .name = "test_nameserver_too_many",
      .code = &test_nameserver_too_many,
      .prio = 4
    },
    {
      .name = "test_nameserver_happypath",
      .code = &test_nameserver_happypath,
      .prio = 4
    },
    {
      .name = "test_undefined_handler",
      .code = &test_undefined_handler,
      .prio = 4
    },
    {
      .name = "message_benchmark",
      .code = &message_benchmark,
      .prio = 6
    },
    {
      .name = "test_mypriority",
      .code = &test_mypriority,
      .prio = 4
    },
    {
      .name = "test_timer_interrupt",
      .code = &test_timer_interrupt,
      .prio = 4
    },
    {
      .name = "test_tx_terminal",
      .code = &test_tx_terminal,
      .prio = 4
    },
    {
      .name = "test_rx_terminal",
      .code = &test_rx_terminal,
      .prio = 4
    },
    {
      .name = "clock_errors",
      .code = &clock_errors,
      .prio = 3
    },
    {
      .name = "clock_accuracy",
      .code = &clock_accuracy,
      .prio = 3
    },
    {
      .name = "clock_syscall_errors",
      .code = &clock_syscall_errors,
      .prio = 3
    },
    {
      .name = "clock_syscall_accuracy",
      .code = &clock_syscall_accuracy,
      .prio = 3
    },
    {
      .name = "test_getcputc_mirror",
      .code = &test_getcputc_mirror,
      .prio = 5
    },
    {
      .name = "test_getcputc_errors",
      .code = &test_getcputc_errors,
      .prio = 5
    },
    {
      .name = "printf_errors",
      .code = &printf_errors,
      .prio = 5
    },
    {
      .name = "printf_happy_path",
      .code = &printf_happy_path,
      .prio = 5
    },
    {
      .name = "test_get_sensors",
      .code = &test_get_sensors,
      .prio = 5
    },
    {
      .name = "test_go_stop",
      .code = &test_go_stop,
      .prio = 5
    },
    { .name = "" } // Keep this guard element at the end of the array.
  };

  do {
    i = 0;
    buf[0] = '\0';

    bwprintf("Enter a test program (q to exit): \n\r");

    c = bwgetc(TERMINAL);
    while (c != '\r') {
      if (c == 8 || c == 127) {
        i = max(0, i - 1);
        putc(TERMINAL, 8);
        putc(TERMINAL, ' ');
        putc(TERMINAL, 8);
      } else {
        buf[i] = c;
        i += 1;
        putc(TERMINAL, c);
      }
      c = bwgetc(TERMINAL);
    }

    buf[i] = '\0';

    bwprintf("\n\r");
    run_test(tests, buf);
  } while (!tstrcmp(buf, "q"));
}
