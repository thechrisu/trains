#ifndef STRING_H
#define STRING_H

#include "char_buffer.h"
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

/**
 * Checks if a given string in an input buffer is a nonnegative decimal number.
 *
 * @param  buf          The buffer to check.
 * @parm   fromOffset   The index in the buffer at which to check if the rest is a number.
 * @return -1           If the string is not a number.
 *         >0           (Actually, the index after the space or after NULL of the next number) on success.
 */
int is_valid_number(char_buffer *buf, unsigned int fromOffset);

/**
 * Parses a nonnegative number. Does not handle failure.
 *
 * @param  buf        The buffer to parse the number from.
 * @param  fromOffset The first (zero-indexed) element that should be parsed.
 * @return The nonnegative number.
 */
unsigned int parse_number(char_buffer *buf, unsigned int start_offset);

#endif /* STRING_H */
