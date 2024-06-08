
#ifndef __MEDUZAOS_STRING
#define __MEDUZAOS_STRING

#include <stdint.h>
#include <stddef.h>

/*
    Returns the length of the C string
    str.
*/
size_t strlen(const char *str);

/*
    Copies the values of num bytes
    from the location pointed to by
    source directly to the memory
    block pointed to by destination.
*/
void *memcpy(void *destination, const void *source, size_t num);

/*
    Compares the first num bytes of
    the block of memory pointed by
    ptr1 to the first num bytes pointed
    by ptr2, returning zero if they all
    match or a value different from zero
    representing which is greater if
    they do not.
*/
int memcmp(const void *ptr1, const void *ptr2, size_t num);

/*
    Appends a copy of the source
    string to the destination string.
    The terminating null character
    in destination is overwritten by
    the first character of source, and
    a null-character is included at the
    end of the new string formed by
    the concatenation of both in destination.
*/
char *strcat(char *destination, const char *source);

/*
    Copies the C string pointed by source
    into the array pointed by destination,
    including the terminating null character
    (and stopping at that point).
*/
char *strcpy(char *destination, const char *source);

char *strtok(char *srcString, char *delim);

#endif