#include "string.h"

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
