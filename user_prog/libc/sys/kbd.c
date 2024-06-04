
#include "kbd.h"
#include "syscall.h"

char buffer[256];

char kbd_getChar(int print)
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

char *kbd_getStr(int print)
{
    char ch = 0;
    int i = 0;
    while (ch != '\n')
    {
        ch = kbd_getChar(print);
        buffer[i] = ch;
        i++;
    }
}
