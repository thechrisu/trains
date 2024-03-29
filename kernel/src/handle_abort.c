#include "tstdlib.h"
#include "myio.h"
#include "syscall/syscall.h"

int num_aborts;

typedef enum abort_mode {
  UNDEFINED,
  PREFETCH_ABORT,
  DATA_ABORT
} abort_mode;

#define PROC_MODE_MASK 0x1F

typedef enum arm_proc_mode {
  USR_MODE = 0x10,
  FIQ_MODE = 0x11,
  IRQ_MODE = 0x12,
  SVC_MODE = 0x13,
  ABT_MODE = 0x17,
  UND_MODE = 0x1B,
  SYS_MODE = 0x1F
} arm_proc_mode;

void print_proc_mode(arm_proc_mode mode) {
  char *proc_mode;
  switch (mode) {
    case USR_MODE:
      proc_mode = "User";
      break;
    case FIQ_MODE:
      proc_mode = "FIQ";
      break;
    case IRQ_MODE:
      proc_mode = "IRQ";
      break;
    case SVC_MODE:
      proc_mode = "Supervisor";
      break;
    case ABT_MODE:
      proc_mode = "Abort";
      break;
    case UND_MODE:
      proc_mode = "Undefined";
      break;
    case SYS_MODE:
      proc_mode = "System";
      break;
    default:
      proc_mode = "Unknown";
      break;
  }
  bwprintf("Processor mode: %s\n\r", proc_mode);
}

void handle_abort(abort_mode abort_type, unsigned int culprit_instruction, trapframe *tf) {
  num_aborts += 1;
  if (num_aborts > 1) while(1);

  *(uint32_t *)(VIC1_BASE + VIC1_ENABLE_OFFSET) = 0x0;
  *(uint32_t *)(VIC2_BASE + VIC2_ENABLE_OFFSET) = 0x0;
  task_descriptor *current_task = get_current_task();
#ifndef TESTING
  logprintf("\n\r\n\r\n\r");
  logprintf("Num ctx sw: %d\n\r", num_ctx_sw);
  logprintf("Abort task id: %d\n\r", current_task->tid);
  print_tf(tf);
  dump_logs();
  // Reset log index so that we don't dump any logs in main.c
  log_index = 0;

  unsigned int cpsr_val; // TODO change to register_t
  __asm__ volatile("MRS %0, cpsr\n\t" : "=r"(cpsr_val));
  int pc_offset = 0;
  char *abort_type_str;
  switch (abort_type) {
    case UNDEFINED:
      pc_offset = -4;
      abort_type_str = "Undefined";
      break;
    case PREFETCH_ABORT:
      pc_offset = -4;
      abort_type_str = "Prefetch abort";
      break;
    case DATA_ABORT:
      pc_offset = -8;
      abort_type_str = "Data abort";
      break;
    default:
      pc_offset = 0;
      abort_type_str = "Unknown abort type";
      break;
  }
#ifndef E2ETESTING
  usage_stats usage;
  syscall_total_proc_usage(&usage);
  print_usage(bwprintf, &usage);
#endif /* E2ETESTING */
  bwprintf("\033[94m\033[5mReceived abort\033[25m\033[39m type: %s\n\r", abort_type_str);
  print_proc_mode((arm_proc_mode)(cpsr_val & PROC_MODE_MASK));
  bwprintf("CPSR: 0x%x\n\r", cpsr_val);
  bwprintf("Offending address: 0x%x\n\r", culprit_instruction + pc_offset);
  syscall_panic();
#endif /* TESTING */
}
