
#include <stdio.h>
#include <sys/syscall.h>

long int ftell(FILE *stream)
{
    struct sysret_t *sys = syscall(SYSCALL_tell, (uint64_t)stream, 0, 0);
    return sys->ret1;
}