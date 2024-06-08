
#ifndef __MEDUZAOS_STDLIB
#define __MEDUZAOS_STDLIB

#include <stdint.h>
#include <stddef.h>

/*
   Terminates the process normally,
   performing the regular cleanup for
   terminating programs.
*/
void exit(int status);

/*
   Allocates a block of size bytes of
   memory, returning a pointer to the
   beginning of the block.
*/
void *malloc(size_t size);

/*
   A block of memory previously allocated
   by a call to malloc, calloc or realloc
   is deallocated, making it available
   again for further allocations.
*/
void free(void *ptr);

int atoi(const char *str);

#endif