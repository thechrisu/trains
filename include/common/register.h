/**
 * @file
 * @brief Defines a register: Is either 32 or 64 bit, depending on the environment.
 */
#ifndef TRAINS_REGISTER_H
#define TRAINS_REGISTER_H

#if TESTING
#include <stdlib.h>
#else
#include "../../src/stdlib.h"
typedef uint32_t register_t;
#endif /* TESTING */

#endif /* TRAINS_REGISTER_H */
