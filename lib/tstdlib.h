/**
 * @file
 * @brief (Re-)Implementation of stdlib functions/types
 */
#ifndef TRAINS_STDLIB_H
#define TRAINS_STDLIB_H

// #include "../test-resources/assert.h"

// #define NULL 0

/**
 * Counts the number of character up to and NOT including the \0. Expects \0 to be there.
 * @param s String.
 * @return Length of string.
 */
int tstrlen(char *s);

/**
 * Compares two strings.
 * @param destination A string to compare.
 * @param source      Another string to compare.
 * @return 1 if they're equal, 0 if not.
 */
int tstrcmp(char *destination, char *source);
int tstrncmp(const char *destination, const char *source, int max_length);

/**
 * Changes a character that matches `[0-9a-fA-F]` into a number between 0 and 15.
 *
 * @param   ch A character.
 * @returns The character's value in base 16, or -1 if the character is invalid.
 */
int a2d(char ch);

/**
 * Calculates the numerical value of part of a character buffer.
 *
 * @param   ch   The first character to include in the numerical value calculation.
 * @param   src  A pointer to a char buffer. `*(*src - 1)` should be equal to `ch`. After
 *               `a2i` returns, `*src` will point to the character after the new value of
 *               `ch`.
 * @param   base The base (_e.g._ 2, 8, 10, or 16).
 * @param   nump `a2i` sets `*nump` to be the parsed number.
 * @returns The first character that isn't part of the parsed number.
 */
char a2i(char ch, char **src, int base, int *nump);

/**
 * Stringifies an unsigned integer.
 *
 * @param num  The number to stringify.
 * @param base The base (_e.g._ 2, 8, 10, or 16).
 * @param bf   The buffer into which to write the string representation of `num`.
 */
void ui2a(unsigned int num, unsigned int base, char *bf);

/**
 * Stringifies a signed base-10 integer.
 *
 * @param num  The number to stringify.
 * @param bf   The buffer into which to write the string representation of `num`.
 */
void i2a(int num, char *bf);

#ifdef TESTING
#else

#define bool _Bool
#endif /* TESTING */

#define true 1
#define false 0

#if HOSTCONFIG
#else

void tmemcpy(void *dst, void *src, unsigned int n);
#ifndef E2ETESTING
void *memcpy(void *dst, const void *src, unsigned int n);
#endif /* E2ETESTING */
void *tmemset(void *s, int c, unsigned int n);

#ifdef TESTING
#include <cstdlib>
#include <stdint.h>
#include <cstring>
#else

// TODO change this via GCC/Clang macros
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int int32_t;
typedef signed long long int64_t;

typedef int32_t register_t;


#endif /* TESTING */
#endif /* HOSTCONFIG */

#endif /* TRAINS_STDLIB_H */
