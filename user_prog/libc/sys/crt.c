
#include "syscall.h"

extern int main(int, char **);

void _start()
{
    unsigned long long argc;
    unsigned long long argv;

    asm volatile ("movq %%rcx, %0" : "=r"(argc));
    asm volatile ("movq %%rdx, %0" : "=r"(argv));

    int code = main((int)argc, (char **)(argv));
    syscall(SYSCALL_exit, code, 0, 0);
    while(1);
}