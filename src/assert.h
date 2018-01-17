/**
 * @file
 * @brief Assert things.
 */
#ifndef KASSERT_H
#define KASSERT_H

#include "attributes.h"
#include "myio.h"
#include "stdlib.h"

#define kassert(expr) __kassert((bool)(expr), __FUNCTION__, __FILE__, __LINE__)

/**
 * If value is false, prints information about where the assertion failed, then exits
 * the kernel.
 *
 * A FAILED ASSERTION ABOVE THE INITIALIZATION CODE AT THE TOP OF MAIN() WILL CAUSE
 * FP AND SP TO BE SET TO UNPREDICTABLE VALUES.
 *
 * To exit the kernel, kassert branches to the kassert_exit label at the end of
 * main(). A FAILED ASSERTION BELOW THIS LABEL WILL LEAD TO AN INFINITE LOOP.
 *
 * To make an assertion in assembly, do something like:
 *
 *     MOV r0, #1
 *     BL __kassert
 *
 * Make sure to save r0 somewhere else before overwriting it. You could also pass
 * other information you want printed if the assertion fails in r1, r2, and r3.
 *
 * @param value       The value to assert.
 * @param caller_name The name of the function in which kassert was called.
 * @param file_name   The name of the file in which kassert was called.
 * @param line_num    The line number in the file at which kassert was called.
 */
void __kassert(bool value, const char * caller_name, const char *file_name, int line_num);

#endif /* KASSERT_H */
