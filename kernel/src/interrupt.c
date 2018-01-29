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
  logputr(tf->r0);
  logputr(tf->r1);
  logputr(tf->r2);
  logputr(tf->r3);
  logputc('\n');
  logputc('\r');
  logputr(tf->r4);
  logputr(tf->r5);
  logputr(tf->r6);
  logputr(tf->r7);
  logputc('\n');
  logputc('\r');
  logputr(tf->r8);
  logputr(tf->r9);
  logputr(tf->r10);
  logputr(tf->fp);
  logputc('\n');
  logputc('\r');
  logputr(tf->ip);
  logputr(tf->sp);
  logputr(tf->lr);
  logputr(tf->pc);
  logputr(tf->k_lr);
  logputr(tf->psr);
  logputc('\n');
  logputc('\r');
#endif /* TESTING */
}

trapframe *handle_interrupt(trapframe *tf, uint32_t pic_status) {
  kassert(tf->sp != 0);
  kassert(tf->fp != 0);
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *loc_kEntry_sys_send = LOC_KENTRY_SYS_SEND;
  volatile int16_t *loc_kEntry_sys_receive = LOC_KENTRY_SYS_RECEIVE;
  volatile int16_t *loc_kEntry_sys_reply = LOC_KENTRY_SYS_REPLY;
  volatile int16_t *is_receive = IS_RECEIVE;
  switch (tf->r0) {
  case SYS_SEND:
    *loc_kEntry_sys_send = get_clockticks();
    // logprintf("(%d) kEntry Send\n\r", *loc_kEntry_sys_send);
    break;
  case SYS_RECEIVE:
    *is_receive = true;
    *loc_kEntry_sys_receive = get_clockticks();
    // logprintf("(%d) kEntry Receive\n\r", *loc_kEntry_sys_receive);
    break;
  case SYS_REPLY:
    *is_receive = false;
    *loc_kEntry_sys_reply = get_clockticks();
    // logprintf("(%d) kEntry Reply\n\r", *loc_kEntry_sys_reply);
    break;
  }
#endif /* CONTEXT_SWITCH_BENCHMARK */
  current_task->tf = tf;
  if ((*((uint32_t*)tf->k_lr-1) & 0x0F000000) != 0x0F000000 && (!((tf->fp & 0xFFFF0000) == 0xF4330000) && ((*((uint32_t*)tf->fp-5)) & 0xF4330000) == 0xF4330000)) {
    logprintf("IH: (CTX %d)\n\r", num_ctx_sw);
    if (pic_status > 0) {
      logprintf("PIC: %d\n\r", pic_status);
    }
    print_tf(tf);
  }
#ifndef TESTING
  register_t cpsr_val;
  __asm__("MRS %0, cpsr\n\t": "=r"(cpsr_val));
#endif /* TESTING */
  kassert((cpsr_val & 0x1F) == 0x13);
  kassert((tf->psr & 0xFF) == 0x10);

  if (pic_status > 0) {
    // Clear interrupt
    *(uint32_t *)0x8081000C = 1;
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
  //logprintf("End of handle_interrupt\n\r");
  //print_tf(tf);
#endif /* TRAPFRAME_DEBUG */
  return tf;
}
