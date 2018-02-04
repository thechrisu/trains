#include "codes.h"
#include "crash.h"
#include "event_data.h"
#include "events.h"
#include "interrupt.h"
#include "myio.h"
#include "mytimer.h"
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

static volatile int num_foobar_stacks = 0;

#if TIMERINTERRUPT_DEBUG
static register_t prev_fp[MAX_TASKS];
#endif /* TIMERINTERRUPT_DEBUG */

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

#if TIMERINTERRUPT_DEBUG
static register_t prev_fp[MAX_TASKS];
#endif /* TIMERINTERRUPT_DEBUG */

trapframe *handle_interrupt(trapframe *tf, uint32_t pic_status) {
  kassert(tf->k_lr != (register_t)0xA1B2C3D4);

  volatile task_descriptor *current_task = get_current_task();
  kassert(tf->sp != 0);
  end_interval(current_task->tid);
  start_interval();
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
#ifndef TESTING
  kassert(tf->sp == (register_t)tf);
#endif /* TESTING */
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

  if (pic_status > 0) {
    //bwprintf("PIC STATUS: %x\n\r", pic_status);
    // Clear interrupt
    int highest_prio_event = -1;
    for (int i = 0; i <= MAX_EVENT_ID; i++) {
      //bwprintf("pic_status: %x, event mask: %x\n\r", pic_status, event_masks[i]);
      if (pic_status & event_masks[i]) {
        highest_prio_event = i;
        break;
      }
    }
    int event_data = -2;
    switch (highest_prio_event) {
      case TIMER_INTERRUPT:
        event_data = 0;
        interrupt_timer_clear();
        ticks += 1;
        break;
      default:
        break; // I <3 GCC
    }
    //bwprintf("H: %d\n\r", highest_prio_event);
    if (highest_prio_event != -1) {
      event_handle(highest_prio_event, event_data);
    }
    return tf;
  }
#endif /* TESTING */

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
    case SYS_MYPRIORITY:
      tf->r0 = syscall_mypriority();
      break;
    case SYS_AWAIT_EVENT:
      tf->r0 = syscall_awaitevent(tf->r1);
#if E2ETESTING && !TIMER_INTERRUPTS
      if (tf->r1 == TIMER_INTERRUPT) { // Simulate instant timer interrupt return.
        event_handle(TIMER_INTERRUPT, 0);
      }
#endif /* E2ETESTING */
      break;
    case SYS_KILL:
      logprintf("Killing task %d\n\r", tf->r1);
      tf->r0 = syscall_kill(tf->r1);
      break;
    case SYS_TOTAL_PROC_USAGE:
      syscall_total_proc_usage((usage_stats*)tf->r1);
      break;
    case SYS_LAST_SECS_PROC_USAGE:
      syscall_last_secs_proc_usage((usage_stats*)tf->r1);
      break;
    case SYS_MY_PROC_USAGE:
      tf->r0 = syscall_my_proc_usage(current_task->tid);
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
