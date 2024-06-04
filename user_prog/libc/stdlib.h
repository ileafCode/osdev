
#ifndef __MEDUZAOS_STDLIB
#define __MEDUZAOS_STDLIB

#include <stdint.h>
#include <stddef.h>

#define EOF -1

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct
{
} FILE;

/*
   Terminates the process normally,
   performing the regular cleanup for
   terminating programs.
*/
void exit(int status);

/*
   Opens the file whose name is
   specified in the parameter *filename*
   and associates it with a stream that
   can be identified in future operations
   by the FILE pointer returned.
*/
FILE *fopen(const char *filename, const char *mode);

/*
   Closes the file associated with the stream and disassociates it.
*/
int fclose(FILE *stream);

/*
   Reads an array of count elements, each
   one with a size of size bytes, from the
   stream and stores them in the block of
   memory specified by ptr.
*/
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);

/*
   Writes an array of count elements, each
   one with a size of size bytes, from the
   block of memory pointed by ptr to the
   current position in the stream.
*/
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);

/*
   Returns the current value of the
   position indicator of the stream.
*/
long int ftell (FILE * stream);

/*
   Sets the position indicator associated
   with the stream to a new position.
*/
int fseek(FILE *stream, long int offset, int origin);

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

#endif