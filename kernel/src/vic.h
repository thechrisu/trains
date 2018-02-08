/*
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

int event_id_to_vic_bit(enum event_id e);
int get_highest_vic_bit_event_id(register_t pic_status);
void enable_in_vic(enum event_id e);
void disable_in_vic(enum event_id e);
void clear_in_vic(enum event_id e);
void enable_vic_bit(int bit);
void disable_vic_bit(int bit);
void vic_maskall();
void setup_vic();

#endif /* VIC_H */

