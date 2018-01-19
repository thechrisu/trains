//
// Created by Christoph Ulshöfer on 2018-01-04.
//

#include "stdlib.h"
#if TESTING
#else

void memcpy(void *dst, void *src, unsigned int n) { // (char *) only used to suppress warning
  // assert(dst != src && ((src < dst && (char *) src + n < (char *) dst) || ((char *) dst + n < (char *) src)));
  unsigned char *srcp = src;
  unsigned char *dstp = dst;
  while (n-- > 0) { *dstp++ = *srcp++; }
}

void *memset(void *s, int c, unsigned int n) {
  unsigned char *p = s;
  while (n-- > 0) { *p++ = (unsigned char) c; }
  return s;
}

int strcmp(char *destination, char *source) {
  while (*destination && *source && (*(destination++) == *(source++)));
  return !(*destination || *source);
}

#endif
