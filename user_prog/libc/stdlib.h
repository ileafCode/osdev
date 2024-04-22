
#ifndef __MEDUZAOS_STDLIB
#define __MEDUZAOS_STDLIB

#include <stdint.h>

typedef struct
{
    // Filename
    char filename[9];
    // Magic
    uint32_t magic; // Must be 0x9F018F02
    // Size in sectors
    uint32_t sizeSector;
    // Data
    uint8_t *data;
} FILE;

// Allocates memory of specified size
void *malloc(uint64_t size);
// Uses malloc() and clears the memory with 0
void *calloc(uint64_t size);
// Frees memory
void free(void *ptr);

FILE *fopen(char *name);
void fclose(FILE *file);

#endif