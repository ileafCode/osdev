
#include <stdio.h>
#include <sys/syscall.h>
#include <string.h>

#define FA_READ 0x01
#define FA_WRITE 0x02
#define FA_OPEN_EXISTING 0x00
#define FA_CREATE_NEW 0x04
#define FA_CREATE_ALWAYS 0x08
#define FA_OPEN_ALWAYS 0x10
#define FA_OPEN_APPEND 0x30

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