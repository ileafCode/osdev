
#include <stdio.h>
#include <sys/syscall.h>

void putchar(int character)
{
    syscall(SYSCALL_writeConsole, (uint64_t)character, 0, 0);
}