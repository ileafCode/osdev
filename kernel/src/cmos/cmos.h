
#pragma once
#include <stdint.h>
#include "../IO.h"

#define CMOS_ADDRESS 0x70 // using outb()
#define CMOS_DATA    0x71

struct time_t
{
    uint8_t epoch;
    uint8_t second;    // Register 0x00 of CMOS
    uint8_t minute;    // Register 0x02 of CMOS
    uint8_t hour;      // Register 0x04 of CMOS
    uint8_t weekday;    // Register 0x06 of CMOS (Sunday = 1)
    uint8_t dayOfMonth; // Register 0x07 of CMOS
    uint8_t month;      // Register 0x08 of CMOS
    uint8_t year;       // Register 0x09 of CMOS
};

time_t rtcGetTime();