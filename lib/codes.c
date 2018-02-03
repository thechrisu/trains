#include "codes.h"
#include "myio.h"

#ifndef TESTING
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

    bwprintf("\033[31mAssertion failed! \"%s\" in function \"%s\" at %s:%d\033[39m\n", expression, caller_name, file_name, line_num);
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
  char reply;
  char msg[tstrlen(c) + 2];
  msg[0] = msg_type;
  tmemcpy(msg + 1, c, tstrlen(c) + 1);

  register_t args[] = {
    NAMESERVER_TASK_ID,
    (register_t)msg,
    (register_t)(tstrlen(c) + 2),
    (register_t)&reply,
    1
  };

  if (software_interrupt(SYS_SEND, 5, args) == -2) {
    return -1;
  }

  switch (reply) {
    case (char)('T' + 128):
      return -2;
    case (char)('S' + 128):
      return -3;
    case (char)('M' + 128):
    case (char)('N' + 128):
      return -4;
    case 'C':
      return 0;
    case (char)('W' + 128):
      return nameserver_panic(c, msg_type);
    default:
      if (msg_type == 'R') {
        return nameserver_panic(c, msg_type);
      } else if (msg_type == 'W') {
        return (int)reply;
      } else {
        return nameserver_panic(c, msg_type);
      }
  }
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
  return ret;
}

int Kill(int tid) {
  register_t args[] = {(register_t)tid};
  return software_interrupt(SYS_KILL, 1, args);
}

#endif /* TESTING */
