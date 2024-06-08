
#ifndef __MEDUZAOS_STDIO
#define __MEDUZAOS_STDIO

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define EOF -1

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct
{
} FILE;

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
long int ftell(FILE *stream);

/*
   Sets the position indicator associated
   with the stream to a new position.
*/
int fseek(FILE *stream, long int offset, int origin);

/*
   Writes the C string pointed by str to
   the standard output (stdout).
*/
void puts(const char *str);


/*
   Composes a string with the same text
   that would be printed if fmt was used
   on printf, but using the elements in
   the variable argument list identified
   by args instead of additional function
   arguments and storing the resulting
   content as a C string in the buffer
   pointed by buf.
*/
int vsprintf(char *buf, const char *fmt, va_list args);

/*
   Writes the C string pointed by format
   to the standard output (stdout). If
   format includes format specifiers
   (subsequences beginning with %), the
   additional arguments following format
   are formatted and inserted in the
   resulting string replacing their
   respective specifiers.
*/
int printf(const char *format, ...);

/*
   Writes a character to the standard
   output (stdout).
*/
void putchar(int character);

#endif