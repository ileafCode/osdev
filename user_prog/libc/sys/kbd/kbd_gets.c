
#include <sys/kbd.h>
#include <sys/syscall.h>

char buffer[256];

char *kbd_gets(int print, int *written)
{
    char ch = 0;
    int i = 0;
    while (ch != '\n')
    {
        ch = kbd_getc(print);
        buffer[i] = ch;
        i++;
    }
    buffer[i - 1] = '\0';
    *written = i - 1;
    return buffer;
}