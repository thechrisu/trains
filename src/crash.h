/**
 * @file
 * @brief Contains a macro for an instruction that crashes reliably (on versatilepb)
 */
#ifndef CRASH_H
#define CRASH_H

#define CRASH() __asm__("mov pc, #0x02000000")

#endif /* CRASH_H */
