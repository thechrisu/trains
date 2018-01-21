#include "codes.h"
#include "myio.h"

#define NULL_ARGS (unsigned int *)0

inline int software_interrupt(unsigned int code, unsigned int argc, unsigned int *argv)__attribute((always_inline));

inline int software_interrupt(unsigned int code, unsigned int argc, unsigned int *argv) {
  register unsigned int arg0 __asm__ ("r0");
  register unsigned int arg1 __asm__ ("r1");
  register unsigned int arg2 __asm__ ("r2");
  register unsigned int arg3 __asm__ ("r3");
  register unsigned int arg4 __asm__ ("r4");
  register unsigned int arg5 __asm__ ("r5");
  register unsigned int arg6 __asm__ ("r6");
  int result;

  arg0 = code;
  if (argc > 0) arg1 = argv[0];
  if (argc > 1) arg2 = argv[1];
  if (argc > 2) arg3 = argv[2];
  if (argc > 3) arg4 = argv[3];
  if (argc > 4) arg5 = argv[4];

  __asm__ volatile (
    "swi 0\n\t"
    "mov %0, r0"
    : "=r" (result)
    : "r" (arg0), "r" (arg1), "r" (arg2), "r" (arg3), "r" (arg4), "r" (arg5), "r" (arg6)
  );

#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("End of software_interrupt\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */

  return result;
}

void Exit() {
  software_interrupt(SYS_EXIT, 0, NULL_ARGS);
  while(1); // To fool gcc into thinking this function does not return.
}

void Pass() {
  software_interrupt(SYS_PASS, 0, NULL_ARGS);
}

int Create(int priority, void (*code)()) {
  unsigned int args[] = { (unsigned int)priority, (unsigned int)code };
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

void __Assert(bool value, const char * caller_name, const char *file_name, int line_num) {
  if (unlikely(!value)) {
    bwprintf("\033[31mAssertion failed! \"%s\" at %s:%d\033[39m\n", caller_name, file_name, line_num);
    /* Call software_interrupt1 instead of Panic to allow inlining. */
    software_interrupt(SYS_PANIC, 0, NULL_ARGS);
  }
}

int Send(int tid, char *msg, int msglen, char *reply, int rplen) {
  unsigned int args[] = {
    (unsigned int)tid,
    (unsigned int)msg,
    (unsigned int)msglen,
    (unsigned int)reply,
    (unsigned int)rplen
  };
  return software_interrupt(SYS_SEND, 5, args);
}

int Receive(int *tid, char *msg, int msglen) {
  unsigned int args[] = {
    (unsigned int)tid,
    (unsigned int)msg,
    (unsigned int)msglen,
  };
  return software_interrupt(SYS_RECEIVE, 3, args);
}

int Reply(int tid, char *reply, int rplen) {
  unsigned int args[] = {
    (unsigned int)tid,
    (unsigned int)reply,
    (unsigned int)rplen
  };
  return software_interrupt(SYS_REPLY, 3, args);
}
