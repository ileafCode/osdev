
#include "stdlib.h"
#include "sys/syscall.h"

#define FA_READ 0x01
#define FA_WRITE 0x02
#define FA_OPEN_EXISTING 0x00
#define FA_CREATE_NEW 0x04
#define FA_CREATE_ALWAYS 0x08
#define FA_OPEN_ALWAYS 0x10
#define FA_OPEN_APPEND 0x30

void exit(int status)
{
    syscall(SYSCALL_exit, status, 0, 0);
    while (1)
        ;
}

FILE *fopen(const char *filename, const char *mode)
{
    // Parse modes
    uint8_t flags = 0;
    int len;
    for (len = 0; mode[len] != 0; len++)
        ;

    if (mode[0] == 'r')
    {
        flags |= FA_READ;
        if (len > 1 && mode[1] == '+')
        {
            flags |= FA_WRITE;
        }
    }
    else if (mode[0] == 'w')
    {
        flags |= FA_CREATE_ALWAYS | FA_WRITE;
        if (len > 1 && mode[1] == '+')
        {
            flags |= FA_READ;
        }
        else if (len > 1 && mode[1] == 'x')
        {
            flags = FA_CREATE_NEW | FA_WRITE;
        }
        if (len > 2 && mode[2] == 'x')
        {
            flags |= FA_READ;
        }
    }
    else if (mode[0] == 'a')
    {
        flags |= FA_OPEN_APPEND | FA_WRITE;
        if (len > 1 && mode[1] == '+')
        {
            flags |= FA_READ;
        }
    }
    else
        return NULL;

    struct sysret_t *sys = syscall(SYSCALL_openFile, (uint64_t)filename, flags, 0);
    FILE *ret = (FILE *)sys->ret1;
    return ret;
}

int fclose(FILE *stream)
{
    struct sysret_t *sys = syscall(SYSCALL_closeFile, (uint64_t)stream, 0, 0);
    return sys->ret1;
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
    size_t newCount = count * size;
    if (newCount == 0)
        return 0;

    struct sysret_t *sys = syscall(SYSCALL_read, (uint64_t)stream, (uint64_t)ptr, newCount);
    return (size_t)sys->ret1;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    size_t newCount = count * size;
    if (newCount == 0)
        return 0;

    struct sysret_t *sys = syscall(SYSCALL_write, (uint64_t)stream, (uint64_t)ptr, newCount);
    return (size_t)sys->ret1;
}

long int ftell(FILE *stream)
{
    struct sysret_t *sys = syscall(SYSCALL_tell, (uint64_t)stream, 0, 0);
    return sys->ret1;
}

int fseek(FILE *stream, long int offset, int origin)
{
    if (origin == SEEK_SET)
    {
        struct sysret_t *sys = syscall(SYSCALL_seek, (uint64_t)stream, offset, 0);
        return sys->ret1;
    }
    else if (origin == SEEK_CUR)
    {
        struct sysret_t *sys = syscall(SYSCALL_seek, (uint64_t)stream, offset + ftell(stream), 0);
        return sys->ret1;
    }
    else if (origin == SEEK_END)
    {
        struct sysret_t *fsize = syscall(SYSCALL_getsize, (uint64_t)stream, 0, 0);
        struct sysret_t *sys = syscall(SYSCALL_seek, (uint64_t)stream, fsize->ret1, 0);
        return sys->ret1;
    }

    return -1;
}

void *malloc(size_t size)
{
    if (size == 0)
        return NULL;
    struct sysret_t *sys = syscall(SYSCALL_malloc, size, 0, 0);
    return (void *)sys->ret1;
}

void free(void *ptr)
{
    if (ptr == NULL)
        return;

    struct sysret_t *sys = syscall(SYSCALL_free, (uint64_t)ptr, 0, 0);
}
