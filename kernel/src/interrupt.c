#include "codes.h"
#include "crash.h"
#include "interrupt.h"
#include "myio.h"
#include "multitasking/messaging.h"
#include "syscall/syscall.h"

#if TESTING
extern void syscall_exit();
extern void syscall_pass();
extern int syscall_mytid();
extern int syscall_myparenttid();
#endif /* TESTING */

int ticks;

void print_tf(trapframe *tf) {
#ifndef TESTING
  bwputr(TERMINAL, tf->r0);
  bwputr(TERMINAL, tf->r1);
  bwputr(TERMINAL, tf->r2);
  bwputr(TERMINAL, tf->r3);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
  bwputr(TERMINAL, tf->r4);
  bwputr(TERMINAL, tf->r5);
  bwputr(TERMINAL, tf->r6);
  bwputr(TERMINAL, tf->r7);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
  bwputr(TERMINAL, tf->r8);
  bwputr(TERMINAL, tf->r9);
  bwputr(TERMINAL, tf->r10);
  bwputr(TERMINAL, tf->fp);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
  bwputr(TERMINAL, tf->ip);
  bwputr(TERMINAL, tf->sp);
  bwputr(TERMINAL, tf->lr);
  bwputr(TERMINAL, tf->pc);
  bwputr(TERMINAL, tf->k_lr);
  bwputr(TERMINAL, tf->psr);
  putc(TERMINAL, '\n');
  putc(TERMINAL, '\r');
#endif /* TESTING */
}

trapframe *handle_interrupt(trapframe *tf, uint32_t pic_status) {
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *loc_kEntry_sys_send = LOC_KENTRY_SYS_SEND;
  volatile int16_t *loc_kEntry_sys_receive = LOC_KENTRY_SYS_RECEIVE;
  volatile int16_t *loc_kEntry_sys_reply = LOC_KENTRY_SYS_REPLY;
  volatile int16_t *is_receive = IS_RECEIVE;
  switch (tf->r0) {
  case SYS_SEND:
    *loc_kEntry_sys_send = get_clockticks();
    // bwprintf("(%d) kEntry Send\n\r", *loc_kEntry_sys_send);
    break;
  case SYS_RECEIVE:
    *is_receive = true;
    *loc_kEntry_sys_receive = get_clockticks();
    // bwprintf("(%d) kEntry Receive\n\r", *loc_kEntry_sys_receive);
    break;
  case SYS_REPLY:
    *is_receive = false;
    *loc_kEntry_sys_reply = get_clockticks();
    // bwprintf("(%d) kEntry Reply\n\r", *loc_kEntry_sys_reply);
    break;
  }
#endif /* CONTEXT_SWITCH_BENCHMARK */
  current_task->tf = tf;

  if (pic_status > 0) {
#if VERSATILEPB
    // Clear interrupt
    *(uint32_t *)0x101E300C = 1;
#else
    // Clear interrupt
    *(uint32_t *)0x8081000C = 1;
#endif
    ticks += 1;
    return tf;
  }

  switch (tf->r0) {
    case SYS_EXIT:
      syscall_exit();
      break;
    case SYS_PASS:
      syscall_pass();
      break;
    case SYS_CREATE:
#ifndef TESTING
      tf->r0 = syscall_create(tf->r1, (void(*)(void))tf->r2);
#endif /* TESTING */
      break;
    case SYS_MYTID:
      tf->r0 = syscall_mytid();
      break;
    case SYS_PARENTTID:
      tf->r0 = syscall_myparent_tid();
      break;
    case SYS_PANIC:
      syscall_panic();
      break;
    case SYS_SEND:
      syscall_send();
      break;
    case SYS_RECEIVE:
      syscall_receive();
      break;
    case SYS_REPLY:
      syscall_reply();
      break;
    case SYS_CACHE_ENABLE:
      syscall_cache_enable();
      break;
    default:
      tf->r0 = 0xABADC0DE;
  }
#if TRAPFRAME_DEBUG
  bwprintf("End of handle_interrupt\n\r");
  print_tf(tf);
#endif /* TRAPFRAME_DEBUG */
  return tf;
}
