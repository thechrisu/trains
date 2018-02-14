#include "tstring.h"

int parse_two_digit_number(char *s) {
  int tens = a2d(s[0]);
  int ones = a2d(s[1]);
  if (ones == -1) {
    ones = tens;
    tens = 0;
  }
  return 10 * tens + ones;
}

int string_starts_with(char *s1, char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2)
      return 0;
    s1 += 1;
    s2 += 1;
  }
  return (!*s1 && *s2) ? 0 : 1;
}

int is_valid_number(char_buffer *buf, unsigned int fromOffset) {
  if (fromOffset >= buf->elems) return -1;
  for (unsigned int i = fromOffset; i < buf->elems; i++) {
    if (buf->data[i] == '\0' || buf->data[i] == ' ')
      return i + 1;
    if (!(buf->data[i] >= '0' && buf->data[i] <= '9')) {
      return -1;
    }
  }
  return buf->elems;
}

unsigned int parse_number(char_buffer *buf, unsigned int start_offset) {
  unsigned int ret = 0;
  for (unsigned int i = start_offset; i < buf->elems; i++) {
    if (buf->data[i] == '\0' || buf->data[i] == ' ') {
      return ret;
    } else {
      ret = ret * 10 + (buf->data[i] - '0');
    }
  }
  return ret;
}
