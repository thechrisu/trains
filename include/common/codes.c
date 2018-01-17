#include "codes.h"
#include "myio.h"

inline int software_interrupt3(unsigned int num, unsigned int arg1_a, unsigned int arg2_a)__attribute((always_inline, visibility("internal")));
inline int software_interrupt2(unsigned int num, unsigned int arg1_a)__attribute((always_inline, visibility("internal")));
inline int software_interrupt1(unsigned int num)__attribute((always_inline, visibility("internal")));

inline int software_interrupt3(unsigned int num, unsigned int arg1_a, unsigned int arg2_a) {
  register unsigned int arg0 __asm__ ("r0");
  register unsigned int arg1 __asm__ ("r1");
  register unsigned int arg2 __asm__ ("r2");
  int result;

  arg0 = num;
  arg1 = arg1_a;
  arg2 = arg2_a;
  __asm__ volatile (
  "swi 0\n\t"
  "mov %0, r0"
  : "=r" (result) : "r" (arg0), "r" (arg1), "r" (arg2));
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("End of software_interrupt\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  return result;
}

inline int software_interrupt2(unsigned int num, unsigned int arg1_a) {
  register unsigned int arg0 __asm__ ("r0");
  register unsigned int arg1 __asm__ ("r1");
  int result;

  arg0 = num;
  arg1 = arg1_a;
  __asm__ volatile (
  "swi 0\n\t"
      "mov %0, r0"
  : "=r" (result) : "r" (arg0), "r" (arg1));
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("End of software_interrupt\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  return result;
}

inline int software_interrupt1(unsigned int num) {
  register unsigned int arg0 __asm__ ("r0");
  int result;

  arg0 = num;
  __asm__ volatile (
  "swi 0\n\t"
      "mov %0, r0"
  : "=r" (result) : "r" (arg0));
#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("End of software_interrupt\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  return result;
}

void Exit() {
  software_interrupt1(SYS_EXIT);
  while(1); // To fool gcc into thinking this function does not return.
}

void Pass() {
  software_interrupt1(SYS_PASS);
}

int Create(int priority, void (*code)()) {
  return software_interrupt3(SYS_CREATE, (unsigned int)priority, (unsigned int)code);
}

int MyTid() {
  return software_interrupt1(SYS_MYTID);
}

int MyParentTid() {
  return software_interrupt1(SYS_PARENTTID);
}

void Abort() {
  software_interrupt1(SYS_ABORT);
  while(1); // To fool gcc into thinking this function does not return.
}

void __Assert(bool value, const char * caller_name, const char *file_name, int line_num) {
  if (unlikely(!value)) {
    bwprintf("\033[31mAssertion failed! \"%s\" at %s:%d\033[39m\n", caller_name, file_name, line_num);
    /* Call software_interrupt1 instead of Abort to allow inlining. */
    software_interrupt1(SYS_ABORT);
  }
}
