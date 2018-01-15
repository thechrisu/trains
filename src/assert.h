#ifndef KASSERT_H
#define KASSERT_H

#include "myio.h"
#include "stdlib.h"

#define kassert(expr) __kassert((bool)(expr), __FUNCTION__, __FILE__, __LINE__)

void __kassert(bool value, const char * caller_name, const char *file_name, int line_num);

#endif /* KASSERT_H */
