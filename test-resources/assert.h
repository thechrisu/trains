#ifndef ASSERT_H
#define ASSERT_H

#include "../src/stdlib.h"
#include "../include/kernel/glue/myio.h"

void __assert(bool expr, const char * caller_name, const char *file_name, int line_num);
#define assert(exp) __assert((bool)(exp), __FUNCTION__, __FILE__, __LINE__)

#endif /* ASSERT_H */
