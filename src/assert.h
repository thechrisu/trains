#ifndef KASSERT_H
#define KASSERT_H

#include "myio.h"
#include "stdlib.h"

#define kassert(expr) __kassert((bool)(expr), __FUNCTION__, __FILE__, __LINE__)

/**
 * If value is false, prints information about where the assertion failed, then exits
 * the kernel.
 *
 * To exit the kernel, kassert branches to the kassert_exit label at the end of
 * handle_interrupt. This relies on the existence of a handle_interrupt stack frame
 * on the kernel stack. Consequently:
 *
 * KASSERT WILL LIKELY NOT WORK IF CALLED FROM THE PARTS OF THE CONTEXT SWITCH THAT ARE
 * WRITTEN IN ASSEMBLY, OR FROM USER CODE.
 *
 * However, kassert should work if called from inside handle_interrupt, or if called from
 * a function that is called inside handle_interrupt.
 * @param value       The value to assert.
 * @param caller_name The name of the function in which kassert was called.
 * @param file_name   The name of the file in which kassert was called.
 * @param line_num    The line number in the file at which kassert was called.
 */
void __kassert(bool value, const char * caller_name, const char *file_name, int line_num);

#endif /* KASSERT_H */
