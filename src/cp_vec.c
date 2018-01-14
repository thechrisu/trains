//
// Created by Christoph Ulshöfer on 2018-01-12.
//

#include "stdlib.h"
#include "myio.h"

extern void enter_kernel();

void cp_vectors() {
  bwprintf("VECTORS");
  extern uint32_t vec_start;
  extern uint32_t vec_end;
  uint32_t *src = &vec_start;
  uint32_t *dst = (uint32_t *)0x0;

  while (src < &vec_end) {
    *dst++ = (*src + 0x4000);
    bwputr(TERMINAL, *src);
    src++;
  }
  putc(TERMINAL, '\n');
}
