
#ifndef __MEDUZAOS_KBD
#define __MEDUZAOS_KBD

#include <stdint.h>
#include <stddef.h>

/*
   Gets a character from stdin
*/
char kbd_getChar(int print);

/*
   Gets a string using kbd_getChar
*/
char *kbd_getStr(int print);

#endif