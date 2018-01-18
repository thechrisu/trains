//
// Created by Christoph Ulshöfer on 2018-01-12.
//

extern void enter_kernel();

void cp_vectors() {

  extern unsigned long vec_start;
  extern unsigned long vec_end;
  unsigned long *src = &vec_start;
  unsigned long *dst = (unsigned long *)0x0;

  while (src < &vec_end) {
    *dst++ = (*src + 0x4000);
    src++;
  }
}
