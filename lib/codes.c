#include "codes.h"

#ifdef TESTING
void __Assert(bool value, const char *expression, const char *caller_name, const char *file_name, int line_num) {
  if (!value) {
    printf("\033[31mAssertion failed! \"%s\" in function \"%s\" at %s:%d\033[39m\n\r", expression, caller_name, file_name, line_num);
  }
}
#else
#ifdef E2ETESTING
#define NAMESERVER_TASK_ID ns_tid
#else
#define NAMESERVER_TASK_ID 2
#endif

#define NULL_ARGS (register_t *)0

inline int software_interrupt(register_t code, register_t argc, register_t *argv)__attribute((always_inline));

inline int software_interrupt(register_t code, register_t argc, register_t *argv) {
  register register_t arg0 __asm__ ("r0");
  register register_t arg1 __asm__ ("r1");
  register register_t arg2 __asm__ ("r2");
  register register_t arg3 __asm__ ("r3");
  register register_t arg4 __asm__ ("r4");
  register register_t arg5 __asm__ ("r5");
  register register_t arg6 __asm__ ("r6");
  int ret;

  arg0 = code;
  if (argc > 0) arg1 = argv[0];
  if (argc > 1) arg2 = argv[1];
  if (argc > 2) arg3 = argv[2];
  if (argc > 3) arg4 = argv[3];
  if (argc > 4) arg5 = argv[4];
  if (argc > 5) arg6 = argv[5];

  __asm__ volatile (
    "swi 0\n\t"
    : "=r" (arg0)
    : "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4), "r" (arg5), "r" (arg6)
  );
  ret = arg0;

#ifdef CONTEXT_SWITCH_DEBUG
  logprintf("End of software_interrupt: %d\n\r", ret);
#endif /* CONTEXT_SWITCH_DEBUG */

  return ret;
}

void Exit() {
  software_interrupt(SYS_EXIT, 0, NULL_ARGS);
  while(1); // To fool gcc into thinking this function does not return.
}

void Pass() {
  software_interrupt(SYS_PASS, 0, NULL_ARGS);
}

int Create(int priority, void (*code)()) {
  register_t args[] = { (register_t)priority, (register_t)code };
  return software_interrupt(SYS_CREATE, 2, args);
}

int MyTid() {
  return software_interrupt(SYS_MYTID, 0, NULL_ARGS);
}

int MyParentTid() {
  return software_interrupt(SYS_PARENTTID, 0, NULL_ARGS);
}

void Panic() {
  software_interrupt(SYS_PANIC, 0, NULL_ARGS);
  while(1); // To fool gcc into thinking this function does not return.
}

void __Assert(bool value, const char *expression, const char *caller_name, const char *file_name, int line_num) {
  if (unlikely(!value)) {
    dump_logs();
    log_index = 0;

    bwprintf("\033[31mAssertion failed! \"%s\" in function \"%s\" at %s:%d\033[39m\n\r", expression, caller_name, file_name, line_num);
    /* Call software_interrupt1 instead of Panic to allow inlining. */
    software_interrupt(SYS_PANIC, 0, NULL_ARGS);
  }
}

int Send(int tid, void *msg, int msglen, void *reply, int rplen) {
  register_t args[] = {
    (register_t)tid,
    (register_t)msg,
    (register_t)msglen,
    (register_t)reply,
    (register_t)rplen
  };
  return software_interrupt(SYS_SEND, 5, args);
}

int Receive(int *tid, void *msg, int msglen) {
  register_t args[] = {
    (register_t)tid,
    (register_t)msg,
    (register_t)msglen,
  };
  return software_interrupt(SYS_RECEIVE, 3, args);
}

int Reply(int tid, void *reply, int rplen) {
  register_t args[] = {
    (register_t)tid,
    (register_t)reply,
    (register_t)rplen
  };
  return software_interrupt(SYS_REPLY, 3, args);
}

int MyPriority() {
  return software_interrupt(SYS_MYPRIORITY, 0, NULL_ARGS);
}

/**
 * Function to call if something goes terribly wrong while sending a message to
 * the nameserver.
 *
 * This shouldn't be used for normal nameserver errors, just situations where
 * the nameserver returns 'W' or something completely unexpected.
 *
 * @param   c        The name that was passed as an argument to the nameserver syscall
 *                   wrapper.
 * @param   msg_type The message type ('R' or 'W').
 * @returns Should never return because it panics. Returns 0xDABBDADD if it does.
 */
int nameserver_panic(char *c, char msg_type) {
  dump_logs();
  log_index = 0;

  bwprintf("Got 'W' from the nameserver. c = %s, msg_type = %d\n\r", c, msg_type);
  software_interrupt(SYS_PANIC, 0, NULL_ARGS);
  return 0xDABBDADD; // Should never reach
}

/**
 * Sends a message to the nameserver.
 *
 * @param   c       The message to send.
 * @param   msg_type 'R' for RegisterAs and 'W' for WhoIs.
 * @returns 0 if everything is OK, the appropriate error code otherwise.
 */
int send_message_to_nameserver(char *c, char msg_type) {
  char msg[tstrlen(c) + 2];
  msg[0] = msg_type;
  tmemcpy(msg + 1, c, tstrlen(c) + 1);

  message reply;

  if (Send(NAMESERVER_TASK_ID, msg, tstrlen(c) + 2, &reply, sizeof(reply)) == -2) {
    return -1;
  }

  Assert(reply.type == REPLY_NAMESERVER);
  int response = reply.msg.nameserver_response;
  return ((uint32_t)response == 0xABAD10DE) ? nameserver_panic(c, msg_type) : response;
}

int RegisterAs(char *c) {
  return send_message_to_nameserver(c, 'R');
}

int WhoIs(char *c) {
  return send_message_to_nameserver(c, 'W');
}

void EnableCaches(bool enable) {
  register_t args[] = {(register_t)enable};
  software_interrupt(SYS_CACHE_ENABLE, 1, args);
}

int AwaitEvent(int event_id) {
  register_t args[] = {(register_t)event_id};
  int ret = software_interrupt(SYS_AWAIT_EVENT, 1, args);
  Assert(ret != 0xBADDA7A);
  return ret;
}

int Kill(int tid) {
  register_t args[] = {(register_t)tid};
  return software_interrupt(SYS_KILL, 1, args);
}

void TotalProcUsage(usage_stats* stats) {
  register_t args[] = {(register_t)stats};
  software_interrupt(SYS_TOTAL_PROC_USAGE, 1, args);
}

void LastSecondsProcUsage(usage_stats* stats) {
  register_t args[] = {(register_t)stats};
  software_interrupt(SYS_LAST_SECS_PROC_USAGE, 1, args);
}

int32_t MyProcUsage() {
  return software_interrupt(SYS_MY_PROC_USAGE, 0, NULL_ARGS);
}

int Time(int tid) {
  message send, reply;
  send.type = MESSAGE_TIME;
  if (Send(tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply)) {
    return reply.msg.reply_time_ticks;
  }
  return -1;
}

int Delay(int tid, int ticks) {
  message send, reply;
  send.type = MESSAGE_DELAY;
  send.msg.message_delay_ticks = ticks;
  if (Send(tid, &send, sizeof(send), &reply, sizeof(reply)) >= 0) {
    Assert(reply.type == REPLY_CLOCK_SERVER_OK || reply.type == REPLY_CLOCK_SERVER_ERROR);
    return reply.type == REPLY_CLOCK_SERVER_ERROR ? -2 : 0;
  }
  return -1;
}

int DelayUntil(int tid, int ticks) {
  message send, reply;
  send.type = MESSAGE_DELAY_UNTIL;
  send.msg.message_delay_until_ticks = ticks;
  if (Send(tid, &send, sizeof(send), &reply, sizeof(reply)) >= 0) {
    Assert(reply.type == REPLY_CLOCK_SERVER_OK || reply.type == REPLY_CLOCK_SERVER_ERROR);
    return reply.type == REPLY_CLOCK_SERVER_ERROR ? -2 : 0;
  }
  return -1;
}

int Putc(int tid, int uart, char ch) { // confoederatio helvetica?
  Assert(uart == TRAIN || uart == TERMINAL);
  message send;
  send.type = MESSAGE_PUTC;
  send.msg.putc = ch;
  if (Send(tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0) {
    return 0;
  }
  return -1;
}

int Getc(int tid, int uart) {
  Assert(uart == TRAIN || uart == TERMINAL);
  message send, reply;
  send.type = MESSAGE_GETC;
  if (Send(tid, &send, sizeof(send), &reply, sizeof(reply)) >= 0) {
    Assert(reply.type == REPLY_GETC);
    return reply.msg.getc;
  }
  return -1;
}

#define PUT(buf, index, c) if (*index < PRINTF_MESSAGE_BUFFER_SIZE) buf[(*index)++] = c

/**
 * Copies a buffer into an output buffer, making sure the output buffer doesn't overflow.
 *
 * @param out       The output buffer.
 * @param out_index printf_putw keeps this equal to the index of the next location in
 *                  the output buffer to which it will write. It makes sure that this
 *                  index doesn't become greater than or equal to PRINTF_MESSAGE_BUFFER_SIZE.
 * @param bf        The buffer to copy from.
 */
void printf_putw(char *out, uint32_t *out_index, char *bf) {
  char ch;
  while ((ch = *bf++)) PUT(out, out_index, ch);
}

/**
 * Prints a format string and variadic arguments into a buffer, while keeping track of
 * how many characters have been printed.
 *
 * @param out       The buffer to print into.
 * @param out_index printf_format keeps this equal to the index of the next location in
 *                  the buffer into which it will print.
 * @param fmt       A format string.
 * @param va        Variadic arguments.
 */
void printf_format(char *out, uint32_t *out_index, char *fmt, va_list va) {
  char bf[12];
  char ch;

  while ((ch = *(fmt++))) {
    if (ch != '%') {
      PUT(out, out_index, ch);
    } else {
      ch = *(fmt++);
      switch (ch) {
        case 0:
          return;
        case 'c':
          PUT(out, out_index, va_arg(va, char));
          break;
        case 's':
          printf_putw(out, out_index, va_arg(va, char *));
          break;
        case 'u':
          ui2a(va_arg(va, unsigned int), 10, bf);
          printf_putw(out, out_index, bf);
          break;
        case 'd':
          i2a(va_arg(va, int), bf);
          printf_putw(out, out_index, bf);
          break;
        case 'x':
          ui2a(va_arg(va, unsigned int), 16, bf);
          printf_putw(out, out_index, bf);
          break;
        case '%':
          PUT(out, out_index, ch);
          break;
      }
    }
  }
}

int Printf(int tid, char *fmt, ...) {
  char buf[PRINTF_MESSAGE_BUFFER_SIZE];
  uint32_t buf_index = 0;

  va_list va;

  va_start(va, fmt);
  printf_format(buf, &buf_index, fmt, va);
  va_end(va);

  if (buf_index >= PRINTF_MESSAGE_BUFFER_SIZE) {
    return -2;
  }
  return PutBytes(tid, buf, buf_index);
}

int PutBytes(int tid, void *bytes, uint32_t bytes_size) {
  message send;
  send.type = MESSAGE_PRINTF;
  send.msg.printf.buf = bytes;
  send.msg.printf.size = bytes_size;

  if (Send(tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0) {
    return 0;
  }
  return -1;
}

#endif /* TESTING */
