
#include <stdlib.h>
#include <sys/syscall.h>

void exit(int status)
{
    syscall(SYSCALL_exit, status, 0, 0);
    while (1)
        ;
}