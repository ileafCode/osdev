
#include <sys/kbd.h>
#include <sys/syscall.h>

char kbd_getc(int print)
{
    char ch = 0;
    while (ch == 0)
    {
        struct sysret_t *ret = syscall(SYSCALL_readScanCode, 0, 0, 0);
        if (ret->ret1 < 0x3A)
        {
            if (print)
                syscall(SYSCALL_writeConsole, ret->ret2, 0, 0);
            ch = (char)ret->ret2;
        }
    }
    return ch;
}