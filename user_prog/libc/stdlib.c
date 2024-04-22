
#include "stdlib.h"
#include "string.h"

void *malloc(uint64_t size)
{
    void *ptr;
    asm volatile ("movq $0, %rbx;");
    asm volatile ("movq %0, %%rcx;" : : "r"(size));
    asm volatile ("int $0x82");
    asm volatile ("movq %%rbx, %0;" : "=r"(ptr));
    return ptr;
}

void *calloc(uint64_t size)
{
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void free(void *ptr)
{
    asm volatile ("movq $1, %rbx;");
    asm volatile ("movq %0, %%rcx;" : : "r"(ptr));
    asm volatile ("int $0x82");
}

FILE *fopen(char *name)
{
    asm volatile("movq %0, %%rcx;" : : "r"(name));
    asm volatile ("movq $1, %rbx; int $0x84");

    uint64_t rbx;
    asm volatile("movq %%rbx, %0;" : "=r"(rbx));
    return (FILE *)rbx;
}

void fclose(FILE *file)
{
    free(file);
}