
#include <stdio.h>
#include <sys/syscall.h>

void puts(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        syscall(SYSCALL_writeConsole, str[i], 0, 0);
}