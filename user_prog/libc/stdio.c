
#include "stdio.h"
#include "string.h"
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

uint8_t getsc()
{
    asm volatile ("movq $0, %rbx; int $0x84");
    uint64_t rbx = 0;
    asm volatile("movq %%rbx, %0;" : "=r"(rbx));
    return (uint8_t)rbx;
}

void putc(char c)
{
    uint64_t char64 = (uint64_t)c;
    asm volatile ("mov %0, %%rbx; int $0x80" : : "a"(char64));
}

void puts(const char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
        putc(str[i]);
}

void printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    int i = 0;
    
    while (format[i] != '\0')
    {
        if (format[i] == '%')
        {
            i++;
            // Handle format specifiers
            switch (format[i])
            {
                case 'o': // Foreground
                {
                    format++;
                    switch (format[i])
                    {
                    /*case '0': { GlobalRenderer->color = 0x00000000; break; } // black
                    case '1': { GlobalRenderer->color = 0x00a32a2a; break; } // dark red
                    case '2': { GlobalRenderer->color = 0x004e9a06; break; } // dark green
                    case '3': { GlobalRenderer->color = 0x00c4a000; break; } // dark yellow
                    case '4': { GlobalRenderer->color = 0x00729fcf; break; } // dark blue
                    case '5': { GlobalRenderer->color = 0x0075507b; break; } // dark magenta
                    case '6': { GlobalRenderer->color = 0x0006989a; break; } // dark cyan
                    case '7': { GlobalRenderer->color = 0x00d3d7cf; break; } // dark white
                    case '8': { GlobalRenderer->color = 0x00555753; break; } // bright black
                    case '9': { GlobalRenderer->color = 0x00ef2929; break; } // bright red
                    case 'A': { GlobalRenderer->color = 0x008ae234; break; } // bright green
                    case 'B': { GlobalRenderer->color = 0x00fce94f; break; } // bright yellow
                    case 'C': { GlobalRenderer->color = 0x0032afff; break; } // bright blue
                    case 'D': { GlobalRenderer->color = 0x00ad7fa8; break; } // bright magenta
                    case 'E': { GlobalRenderer->color = 0x0034e2e2; break; } // bright cyan
                    case 'F': { GlobalRenderer->color = 0x00FFFFFF; break; } // bright white */
                    }
                    break;
                }
                case 'd':
                {
                    int value = va_arg(args, int);
                    const char* str = int_to_string((int64_t)value);
                    puts(str);
                    break;
                }
                
                case 's':
                {
                    const char* value = va_arg(args, const char*);
                    puts(value);
                    break;
                }

                case 'x':
                {
                    uint64_t value = va_arg(args, uint64_t);
                    const char* str = uint64_to_hstring((uint64_t)value);
                    puts(str);
                    break;
                }

                case 'f':
                {
                    double value = va_arg(args, double);
                    const char* str = double_to_string((double) value);
                    puts(str);
                    break;
                }

                case 'c':
                {
                    char value = (char) va_arg(args, int);
                    putc(value);
                    break;
                }
                
                default:
                    putc(format[i]);
                    break;
            }
        }
        else
        {
            putc(format[i]);
        }

        i++;
    }
    
    va_end(args);
}