/**
 * @file
 * @brief Methods for setting/clearing things in the PIC/VIC/whatever.
 */
#ifndef VIC_H
#define VIC_H

#include "event_data.h"
#include "tstdlib.h"
#include "io.h"

// TODO install_handler(int interrupt_num);

#define UART1_BIT 52
#define UART2_BIT 54

/**
 * Want to read/write something in the VIC, but don't know which bit to modify?
 * We got you covered!
 *
 * @param e    Event id to convert to VIC bit.
 * @return     The VIC bit (as laid out in the Versatile/PB and EP9302 manuals),
 *             or -1 if an unsupported event was passed.
 */
int event_id_to_vic_bit(enum event_id e);

/**
 * Rotates through the event mask to pick the one with the lowest event id.
 * If the VIC bit and the event mask overlap, return the event id of the current event mask.
 * @param pic_status  PIC/VIC status register.
 * @return            Event id of the highest-priority (= lowest event)
 *                    or -1 if there's no match.
 */
int get_highest_vic_bit_event_id(register_t pic_status);

/**
 * Enables an event in the VIC,
 * by first looking up the event id and then setting the right bit.
 *
 * @param e           Event to enable
 */
void enable_in_vic(enum event_id e);

/**
 * Disables an event in the VIC,
 * by first looking up the event id and then setting the right bit.
 *
 * @param e           Event to disable
 */
void disable_in_vic(enum event_id e);

/**
 * Disables an event in the VIC. FAILS SILENTLY.
 * In practice, we just disable the VIC in one instruction.
 *
 * @param e           Event to disable.
 */
void clear_in_vic(enum event_id e);

/**
 * Enables one bit in the VIC.
 *
 * @param bit to enable
 */
void enable_vic_bit(int bit);

/**
 * Disables one bit in the VIC.
 *
 * @param bit to disable
 */
void disable_vic_bit(int bit);

/**
 * Masks + clears all bits in both VICs.
 */
void vic_maskall();

/**
 * Needs to be called on system start:
 * Sets up the event masks array, enables the vic bits used for our train kernel.
 */
void setup_vic();

#endif /* VIC_H */

