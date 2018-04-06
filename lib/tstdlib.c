#include "tstdlib.h"

void tmemcpy(void *dst, const void *src, unsigned int n) { // (char *) only used to suppress warning
  // assert(dst != src && ((src < dst && (char *) src + n < (char *) dst) || ((char *) dst + n < (char *) src)));
  unsigned char *srcp = (unsigned char *)src;
  unsigned char *dstp = (unsigned char *)dst;

// only works on 32bit
#ifndef TESTING
  if (!((register_t)dst & 0x3) && !((register_t)src & 0x3)) {
    long *srcpl = (long *)src;
    long *dstpl = (long *)dst;
    while (n >= 4) {
      *dstpl++ = *srcpl++;
      n -= 4;
    }
    dstp = (unsigned char *)dstpl;
    srcp = (unsigned char *)srcpl;
  }
#endif /* TESTING */
  while (n-- > 0) { *dstp++ = *srcp++; }
}

void *memcpy(void *dst, const void *src, unsigned int n) { // (char *) only used to suppress warning
  // assert(dst != src && ((src < dst && (char *) src + n < (char *) dst) || ((char *) dst + n < (char *) src)));
  tmemcpy(dst, src, n);
  return dst;
}

void *tmemset(void *s, int c, unsigned int n) {
  unsigned char *p = (unsigned char *)s;
  while (n-- > 0) { *p++ = (unsigned char) c; }
  return s;
}

void *memset(void *s, int c, unsigned int n) {
  return tmemset(s, c, n);
}

int tstrncmp(const char *destination, const char *source, int max_length) {
  for (int i = 0; i < max_length; i++) {
    if (destination[i] == '\0' && source[i] == '\0') return 0;
    if (destination[i] > source[i]) return 1;
    if (destination[i] < source[i]) return -1;
  }
  return 0;
}

int tstrcmp(char *destination, char *source) {
  int i = 0;
  while (true) {
    if (destination[i] == '\0' && source[i] == '\0') return 1;
    if (destination[i] != source[i]) return 0;
    i += 1;
  }
}

int tstrlen(char *s) {
  int result = 0;
  while (*s) {
    s += 1;
    result += 1;
  }
  return result;
}

int a2d(char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  return -1;
}

char a2i(char ch, char **src, int base, int *nump) {
  int num, digit;
  char *p;

  p = *src;
  num = 0;
  while ((digit = a2d(ch)) >= 0) {
    if (digit > base) break;
    num = num * base + digit;
    ch = *p++;
  }
  *src = p;
  *nump = num;
  return ch;
}

void ui2a(unsigned int num, unsigned int base, char *bf) {
  int n = 0;
  int dgt;
  unsigned int d = 1;

  while ((num / d) >= base) d *= base;
  while (d != 0) {
    dgt = num / d;
    num %= d;
    d /= base;
    if (n || dgt > 0 || d == 0) {
      *bf++ = dgt + (dgt < 10 ? '0' : 'a' - 10);
      ++n;
    }
  }
  *bf = 0;
}

void i2a(int num, char *bf) {
  if (num < 0) {
    num = -num;
    *bf++ = '-';
  }
  ui2a(num, 10, bf);
}

float tinvsqrt(float number) {
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"

  i  = * ( long * ) &y;
  i  = 0x5f3759df - ( i >> 1 );
  y  = * ( float * ) &i;

  #pragma GCC diagnostic pop

  y  = y * ( threehalfs - ( x2 * y * y ) );
  return y;
}
