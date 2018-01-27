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

#ifdef TESTING
#else

#define bool _Bool
#endif /* TESTING */

#define true 1
#define false 0

#if HOSTCONFIG
#else

void tmemcpy(void *dst, void *src, unsigned int n);
void *tmemset(void *s, int c, unsigned int n);

#ifdef TESTING
#include <boost/integer.hpp>
#include <cstring>
#else
void memcpy(void *dst, void *src, unsigned int n);

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
