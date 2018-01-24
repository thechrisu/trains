#include "kassert.h"
#include "myio.h"
#include "stdlib.h"

extern void enter_kernel();

bool cp_vectors_called = false;

void cp_vectors() {
  kassert(!cp_vectors_called);

  cp_vectors_called = true;

#ifdef CONTEXT_SWITCH_DEBUG
  bwprintf("VECTORS ");
#endif /* CONTEXT_SWITCH_DEBUG */
  extern uint32_t vec_start;
  extern uint32_t vec_end;
  uint32_t *src = &vec_start;
  uint32_t *dst = (uint32_t *)0x0;

  while (src < &vec_end) {
    *dst++ = (*src + 0x4000);
#ifdef CONTEXT_SWITCH_DEBUG
    bwputr(TERMINAL, *src);
#endif /* CONTEXT_SWITCH_DEBUG */
    src++;
  }

#ifdef CONTEXT_SWITCH_DEBUG
  putc(TERMINAL, '\n');
#endif /* CONTEXT_SWITCH_DEBUG */
}
