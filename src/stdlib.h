/**
 * @file
 * @brief (Re-)Implementation of stdlib functions/types
 */
#ifndef TRAINS_STDLIB_H
#define TRAINS_STDLIB_H

// #include "../test-resources/assert.h"

// #define NULL 0

#ifdef TESTING
#else

#define bool _Bool

/**
 * Counts the number of character up to and NOT including the \0. Expects \0 to be there.
 * @param s String.
 * @return Length of string.
 */
int strlen(char *s);

/**
 * Compares two strings.
 * @param destination A string to compare.
 * @param source      Another string to compare.
 * @return 1 if they're equal, 0 if not.
 */
int strcmp(char *destination, char *source);
int strncmp(const char *destination, const char *source, int max_length);

#endif

#define true 1
#define false 0

#if HOSTCONFIG
#else

#ifdef TESTING

#include <boost/integer.hpp>
#include <cstring>
#define my_memset(dst, src, n) printf("%p, %p, %d", dst, src, n)
#define my_memcpy(s, c, n) printf("%p, %d, %u", s, c, n)
#else

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int int32_t;
typedef signed long long int64_t;

typedef int32_t register_t;

void memcpy(void *dst, void *src, unsigned int n);
void *memset(void *s, int c, unsigned int n);

#endif
#endif

#endif /* TRAINS_STDLIB_H */
