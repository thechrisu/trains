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

int strncmp(const char *destination, const char *source, int max_length) {
  for (int i = 0; i < max_length; i++) {
    if (destination[i] == '\0' && source[i] == '\0') return 0;
    if (destination[i] > source[i]) return 1;
    if (destination[i] < source[i]) return -1;
  }
  return 0;
}

int strcmp(char *destination, char *source) {
  int i = 0;
  while (true) {
    if (destination[i] == '\0' && source[i] == '\0') return 1;
    if (destination[i] != source[i]) return 0;
    i += 1;
  }
}

int strlen(char *s) {
  int result = 0;
  while (*s) {
    s += 1;
    result += 1;
  }
  return result;
}

#endif
