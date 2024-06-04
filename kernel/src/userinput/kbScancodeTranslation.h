#pragma once
#include <stdint.h>

namespace QWERTYKeyboard
{

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39
#define Caps 0x3A
#define LeftControl 0x1D

    extern const char ASCII_table_cl[];
    extern const char ASCII_table_shift[];
    extern const char ASCII_table_lower[];
    char Translate(uint8_t scancode, bool uppercase, bool caps);
}