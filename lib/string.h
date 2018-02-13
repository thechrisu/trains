#ifndef STRING_H
#define STRING_H

#include "tstdlib.h"

/**
 * Parses the first one to two characters of s
 * as if they are a number, then returns the result.
 *
 * @param s  String to be parsed as two-digit number (expects NULL terminator).
 * @return   A (hopefully) correctly parsed number.
 */
int parse_two_digit_number(char *s);

/**
 * Returns 1 if s1 starts with s2 and 0 otherwise.
 *
 * @param s1  Input string.
 * @param s2  The prefix to match against.
 *
 * @return 1 if s2 is a prefix of s1, e.g. string_starts_with("TESTER", "TEST") == 1.
 */
int string_starts_with(char *s1, char *s2);

#endif /* STRING_H */
