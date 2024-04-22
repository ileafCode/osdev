
#ifndef __MEDUZAOS_STDIO
#define __MEDUZAOS_STDIO

#include <stdint.h>

uint8_t getsc();

void putc(char c);
void puts(const char* str);
void printf(const char* format, ...);

#endif