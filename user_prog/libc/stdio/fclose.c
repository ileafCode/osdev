
#include <stdio.h>
#include <sys/syscall.h>

int fclose(FILE *stream)
{
    struct sysret_t *sys = syscall(SYSCALL_closeFile, (uint64_t)stream, 0, 0);
    return sys->ret1;
}