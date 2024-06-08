
#include <stdlib.h>
#include <sys/syscall.h>

void *malloc(size_t size)
{
    if (size == 0)
        return NULL;
    struct sysret_t *sys = syscall(SYSCALL_malloc, size, 0, 0);
    return (void *)sys->ret1;
}