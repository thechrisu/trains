/**
 * @file
 * @brief Defines macros for triggering certain aborts when in user mode.
 */
#ifndef TRAINS_USER_DATA_ABORT_H
#define TRAINS_USER_DATA_ABORT_H

#define DATA_ABORT()   __asm__("MSR cpsr_c, #0x1F\n\t");

#endif /* TRAINS_USER_DATA_ABORT_H */
