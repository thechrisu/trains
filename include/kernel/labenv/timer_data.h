//
// Created by Christoph Ulshöfer on 2018-01-06.
//

#ifndef TRAINS_TIMER_DATA_H
#define TRAINS_TIMER_DATA_H

#include "../../src/stdlib.h"

struct my_time {
  uint32_t min;
  uint32_t sec;
  uint32_t dsec;
};

typedef struct my_time my_time;

// 32 bit timer
#define TIMER3_CLK           0x8         // 1: 508kHz, 0: 2kHz. default: 0
#define TIMER3_ENABLE        0x80
#define TIMER3_MODE          0x40        // 1: periodic (reloads val), 0: free running (0xFFFFFFFF)
#define TIMER3_LOAD          0x80810080  // 32bits
#define TIMER3_VALUE         0x80810084  // 32 bits
#define TIMER3_CTRL          0x80810088  // 32 bits
#define TIMER3_CLEAR         0x8081008C  // 1 bit

// 40 bit timer
#define TIMER4_VAL_LO        0x80810060  // 32 bits
#define TIMER4_HI_ONOFF      0x80810064  // 9 bits (technically unsigned short)
#define TIMER4_HI_VAL_MSK    0xFF
#define TIMER4_ONOFF_MSK     0x100       // 1: Enable, 0: disable timer

#endif //TRAINS_TIMER_DATA_H
