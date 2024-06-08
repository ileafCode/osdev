
#include <stdlib.h>
#include <sys/syscall.h>

void free(void *ptr)
{
    if (ptr == NULL)
        return;
    syscall(SYSCALL_free, (uint64_t)ptr, 0, 0);
}