//
// Created by Christoph Ulshöfer on 2018-01-04.
//

#ifndef TRAINS_STDLIB_H
#define TRAINS_STDLIB_H

// #include "../test-resources/assert.h"

// #define NULL 0

#ifdef TESTING
#else

#define bool _Bool

#endif

#define true 1
#define false 0

#if HOSTCONFIG
#else

#ifdef TESTING

#include <boost/integer.hpp>
#define my_memset(dst, src, n) printf("%p, %p, %d", dst, src, n)
#define my_memcpy(s, c, n) printf("%p, %d, %u", s, c, n)

#else

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
// typedef signed int int16_t;
// typedef signed long int int32_t;
typedef signed long long int64_t;

void my_memcpy(void *dst, void *src, unsigned int n);
void *my_memset(void *s, int c, unsigned int n);

#endif
#endif

#endif /* TRAINS_STDLIB_H */
