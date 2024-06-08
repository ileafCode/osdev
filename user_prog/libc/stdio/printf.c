
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

const uint32_t conv[16] = {
    // Darker colors
    0x00000000, // BLACK
    0x000000AA, // BLUE
    0x0000AA00, // GREEN
    0x0000AAAA, // CYAN
    0x00AA0000, // RED
    0x00AA00AA, // MAGENTA
    0x00AA5500, // YELLOW
    0x00AAAAAA, // LIGHT GREY

    // Brighter colors
    0x00555555, // BLACK
    0x005555FF, // BLUE
    0x0055FF55, // GREEN
    0x0055FFFF, // CYAN
    0x00FF5555, // RED
    0x00FF55FF, // MAGENTA
    0x00FFFF55, // YELLOW
    0x00FFFFFF, // WHITE
};

uint32_t curFG = 0x00FFFFFF;
uint32_t curBG = 0x00000000;

void parse_ansi(const char *text)
{
    char ending;
    const char *pos = text;
    while (*pos)
    {
        if (*pos == '\033' && *(pos + 1) == '[')
        {
            // Found an ANSI escape sequence
            pos += 2; // Skip \033[
            char codes[16];
            long unsigned int i = 0;
            while (*pos != 'm' && i < sizeof(codes) - 1)
            {
                codes[i++] = *pos++;
            }

            ending = *pos;
            codes[i] = '\0';
            pos++;

            // Process the escape codes
            int code;
            char *token = strtok(codes, ";");
            while (token)
            {
                code = atoi(token);
                if (code == 0)
                {
                    curFG = 0x00FFFFFF;
                    curBG = 0x00000000;
                    syscall(SYSCALL_writeConsole, 0x1F, curFG, curBG);
                }
                else
                {
                    if (code >= 30 && code < 38) // 30 - 37
                    {
                        int colorNum = code - 30;
                        curFG = conv[colorNum];
                        syscall(SYSCALL_writeConsole, 0x1F, curFG, curBG);
                    }
                    else if (code >= 40 && code < 48) // 40 - 47
                    {
                        int colorNum = code - 40;
                        curBG = conv[colorNum];
                        syscall(SYSCALL_writeConsole, 0x1F, curFG, curBG);
                    }
                    else if (code >= 90 && code < 98) // 90 - 97
                    {
                        int colorNum = code - 90;
                        curFG = conv[colorNum + 8];
                        syscall(SYSCALL_writeConsole, 0x1F, curFG, curBG);
                    }
                    else if (code >= 100 && code < 108) // 100 - 107
                    {
                        int colorNum = code - 100;
                        curBG = conv[colorNum + 8];
                        syscall(SYSCALL_writeConsole, 0x1F, curFG, curBG);
                    }
                }
                token = strtok(NULL, ";");
            }
        }
        else
        {
            putchar(*pos++);
        }
    }
}

char buf[4096];

int printf(const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    parse_ansi(buf);
    //puts((const char *)buf);
    return i;
}