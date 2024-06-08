
#include <stdio.h>
#include <sys/syscall.h>

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