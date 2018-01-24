/**
 * @file
 * @brief Defines macros for triggering certain aborts when in user mode.
 */
#ifndef TRAINS_USER_DATA_ABORT_H
#define TRAINS_USER_DATA_ABORT_H

/**
 * Triggers UNDEFINED.
 * See http://bear.ces.cwru.edu/eecs_382/ARM7-TDMI-manual-pt2.pdf page 2 of the PDF.
 */
void UNDEFINED() {
  __asm__("mov pc, %0\n\t" :: "r"(i));
}

#endif /* TRAINS_USER_DATA_ABORT_H */
