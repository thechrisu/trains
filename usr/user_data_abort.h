/**
 * @file
 * @brief Defines macros for triggering certain aborts when in user mode.
 */
#ifndef TRAINS_USER_DATA_ABORT_H
#define TRAINS_USER_DATA_ABORT_H

void UNDEFINED() {
  int *i = 0x01000000;
  *i = 0x06000010;
  __asm__("mov pc, %0\n\t" :: "r"(i));
}

#endif /* TRAINS_USER_DATA_ABORT_H */
