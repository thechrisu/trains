#include "tstdlib.h"

void tmemcpy(void *dst, void *src, unsigned int n) { // (char *) only used to suppress warning
  // assert(dst != src && ((src < dst && (char *) src + n < (char *) dst) || ((char *) dst + n < (char *) src)));
  unsigned char *srcp = (unsigned char *)src;
  unsigned char *dstp = (unsigned char *)dst;
  while (n-- > 0) { *dstp++ = *srcp++; }
}

void memcpy(void *dst, void *src, unsigned int n) { // (char *) only used to suppress warning
  // assert(dst != src && ((src < dst && (char *) src + n < (char *) dst) || ((char *) dst + n < (char *) src)));
  tmemcpy(dst, src, n);
}

void *tmemset(void *s, int c, unsigned int n) {
  unsigned char *p = (unsigned char *)s;
  while (n-- > 0) { *p++ = (unsigned char) c; }
  return s;
}

void *memset(void *s, int c, unsigned int n) {
  tmemset(s, c, n);
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
