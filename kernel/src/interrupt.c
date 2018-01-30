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
extern int software_interrupt(register_t code, register_t argc, register_t *argv);

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

static volatile int num_foobar_stacks = 0;
static register_t prev_fp[MAX_TASKS];

trapframe *handle_interrupt(trapframe *tf, uint32_t pic_status) {
  kassert(tf->k_lr != 0xA1B2C3D4);

  volatile task_descriptor *current_task = get_current_task();
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
  kassert(tf->sp == (int)tf);
  current_task->tf = tf;
#if TIMERINTERRUPT_DEBUG
  if (current_task->tid == 3) {
    if ((tf->fp & 0xFFFF0000) != 0xF4330000) { // && (tf->k_lr < (uint32_t)&software_interrupt || tf->k_lr > (uint32_t)(&software_interrupt) + 1000)) {
      bool found_junk = false;
      for (register_t* i = tf->fp - 4; i >= tf->sp + 72 && !found_junk; i--) {
        found_junk |= (*i & 0xFFFF0000) == 0xF4330000;
      }
      if (found_junk) {
        logprintf("Old fp: %x, New fp: %x, Sp: %x, tid: %d, ctx: %d, PIC: %d, TF: %x, klr: %x\n\r", prev_fp[current_task->tid], tf->fp, tf->sp + 72, current_task->tid, num_ctx_sw, pic_status, tf, tf->k_lr);
        print_tf(tf);
        for (register_t* i = tf->fp - 4; i >= tf->sp + 72; i--) {
          logprintf("%x ", *i);
        }
        logprintf("\n\r");
        logprintf("\n\r");
      }
    }
    prev_fp[current_task->tid] = tf->fp;
  }
#endif /* TIMERINTERRUPT_DEBUG */

#ifndef TESTING
  register_t cpsr_val;
  __asm__("MRS %0, cpsr\n\t": "=r"(cpsr_val));
  kassert((cpsr_val & 0x1F) == 0x13);
  kassert((tf->psr & 0xFF) == 0x10);
#endif /* TESTING */

  if (pic_status > 0) {
    // Clear interrupt
    *(uint32_t *)(TIMER2_BASE + CLR_OFFSET) = 1;
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
  logprintf("End of handle_interrupt\n\r");
  print_tf(tf);
#endif /* TRAPFRAME_DEBUG */
  return tf;
}
