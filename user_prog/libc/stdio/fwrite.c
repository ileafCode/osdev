
#include <stdio.h>
#include <sys/syscall.h>

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
    size_t newCount = count * size;
    if (newCount == 0)
        return 0;

    struct sysret_t *sys = syscall(SYSCALL_write, (uint64_t)stream, (uint64_t)ptr, newCount);
    return (size_t)sys->ret1;
}