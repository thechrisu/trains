#include "stdlib.h"
#include "myio.h"
#include "syscall/syscall.h"

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
  }
#ifndef TESTING
  logprintf("Processor mode: %s\n\r", proc_mode);
#endif /* TESTING */
}

void handle_abort(abort_mode abort_type, unsigned int culprit_instruction) {
#ifndef TESTING
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
  }
  logprintf("\033[94m\033[5mReceived abort\033[25m\033[39m type: %s\n\r", abort_type_str);
  print_proc_mode((arm_proc_mode)(cpsr_val & PROC_MODE_MASK));
  logprintf("CPSR: 0x%x\n\r", cpsr_val);
  logprintf("Offending address: 0x%x\n\r", culprit_instruction + pc_offset);
  syscall_panic();
#endif /* TESTING */
}
