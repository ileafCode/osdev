
#ifndef __MEDUZAOS_KBD
#define __MEDUZAOS_KBD

#include <stdint.h>
#include <stddef.h>

/*
   Gets a character from stdin
*/
char kbd_getc(int print);

/*
   Gets a string from stdin
*/
char *kbd_gets(int print, int *written);

#endif