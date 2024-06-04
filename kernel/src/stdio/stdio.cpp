
#include "stdio.h"
#include "../IO.h"
#include "../scheduling/task/sched.h"

void puts(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        GlobalRenderer->PutChar((int)str[i]);
}

void debug_puts(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
        outb(0xE9, str[i]);
}

#define is_digit(c) ((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
    int i = 0;

    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

#define ZEROPAD 1  /* pad with zero */
#define SIGN 2     /* unsigned/signed long */
#define PLUS 4     /* show plus */
#define SPACE 8    /* space if plus */
#define LEFT 16    /* left justified */
#define SPECIAL 32 /* 0x */
#define SMALL 64   /* use 'abcdef' instead of 'ABCDEF' */

#define do_div(n, base) ({ \
int64_t __res; \
__asm__("divq %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
__res; })

static char *fnumber(char *str, double num, int size, int precision, int type)
{
    if (precision < 0)
        precision = 2;
    
    char c, sign, tmp[36];
    const char *digits = "0123456789";
    int i, int_part;
    double frac_part;
    int length = 0;  // Length of the number string
    char *start = str;

    // Handle the sign
    if (type & SIGN && num < 0) {
        sign = '-';
        num = -num;
    } else {
        sign = (type & SPACE) ? ' ' : 0;
    }

    // Handle padding character
    c = (type & ZEROPAD) ? '0' : ' ';

    // Split into integer and fractional parts
    int_part = (int)num;
    frac_part = num - int_part;

    // Convert integer part
    i = 0;
    if (int_part == 0) {
        tmp[i++] = '0';
    } else {
        while (int_part != 0) {
            tmp[i++] = digits[int_part % 10];
            int_part /= 10;
        }
    }

    // Calculate length of the integer part
    int int_length = i;

    // Add sign length
    if (sign) {
        length++;
    }

    // Calculate total length
    length += int_length + (precision > 0 ? 1 + precision : 0); // +1 for decimal point if precision > 0

    // Add padding before number if not left-aligned
    if (!(type & LEFT)) {
        while (size-- > length) {
            *str++ = c;
        }
    }

    // Add sign if present
    if (sign) {
        *str++ = sign;
    }

    // Add integer part to output string
    while (i-- > 0) {
        *str++ = tmp[i];
    }

    // Add decimal point and fractional part if precision is specified
    if (precision > 0) {
        *str++ = '.';

        // Convert fractional part
        for (i = 0; i < precision; i++) {
            frac_part *= 10;
            int digit = (int)frac_part;
            *str++ = digits[digit];
            frac_part -= digit;
        }
    }

    // Add padding after number if left-aligned
    if (type & LEFT) {
        while (size-- > length) {
            *str++ = ' ';
        }
    }

    return str;
}

static char *number(char *str, int64_t num, int64_t base, int64_t size, int64_t precision, int64_t type)
{
    char c, sign, tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;

    if (type & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ';
    if (type & SIGN && num < 0)
    {
        sign = '-';
        num = -num;
    }
    else
        sign = ((type & SPACE) ? ' ' : 0);
    if (sign)
        size--;
    if (type & SPECIAL)
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
        while (num != 0)
            tmp[i++] = digits[do_div(num, base)];
    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type & (ZEROPAD + LEFT)))
        while (size-- > 0)
            *str++ = ' ';
    if (sign)
        *str++ = sign;
    if (type & SPECIAL)
        if (base == 8)
            *str++ = '0';
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digits[33];
        }
    if (!(type & LEFT))
        while (size-- > 0)
            *str++ = c;
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    int i;
    char *str;
    char *s;
    int *ip;

    int flags; /* flags to number() */

    int field_width; /* width of output field */
    int precision;   /* min. # of digits for integers; max
                number of chars for from string */
    int qualifier;   /* 'h', 'l', or 'L' for integer fields */

    for (str = buf; *fmt; ++fmt)
    {
        if (*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }

        /* process flags */
        flags = 0;
    repeat:
        ++fmt; /* this also skips first '%' */
        switch (*fmt)
        {
        case '-':
            flags |= LEFT;
            goto repeat;
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        /* get field width */
        field_width = -1;
        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++fmt;
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            ++fmt;
        }

        switch (*fmt)
        {
        case 'v': // Foreground
        {
            *str++ = '%';
            *str++ = 'v';
            break;
        }
        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char)va_arg(args, int);
            while (--field_width > 0)
                *str++ = ' ';
            break;

        case 's':
            s = va_arg(args, char *);
            len = strlen(s);
            if (precision < 0)
                precision = len;
            else if (len > precision)
                len = precision;

            if (!(flags & LEFT))
                while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            break;

        case 'o':
            str = number(str, va_arg(args, unsigned long), 8,
                         field_width, precision, flags);
            break;

        case 'p':
            if (field_width == -1)
            {
                field_width = 8;
                flags |= ZEROPAD;
            }
            str = number(str,
                         (unsigned long)va_arg(args, void *), 16,
                         field_width, precision, flags);
            break;

        case 'x':
            flags |= SMALL;
        case 'X':
            str = number(str, va_arg(args, unsigned long), 16,
                         field_width, precision, flags);
            break;

        case 'f':
            str = fnumber(str, va_arg(args, double), field_width, precision, flags);
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            str = number(str, va_arg(args, unsigned long), 10,
                         field_width, precision, flags);
            break;
        case 'n':
            ip = va_arg(args, int *);
            *ip = (str - buf);
            break;

        default:
            if (*fmt != '%')
                *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            break;
        }
    }
    *str = '\0';
    return str - buf;
}

char buf[4096];

int printf(const char *fmt, ...)
{
    sched_lock();
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    for (int i = 0; buf[i] != 0; i++)
    {
        if (buf[i] == '%')
        {
            i++;
            if (buf[i] == 'v')
            {
                i++;
                switch (buf[i])
                {
                case '0':
                {
                    GlobalRenderer->color = 0x00000000;
                    break;
                } // black
                case '1':
                {
                    GlobalRenderer->color = 0x00a32a2a;
                    break;
                } // dark red
                case '2':
                {
                    GlobalRenderer->color = 0x004e9a06;
                    break;
                } // dark green
                case '3':
                {
                    GlobalRenderer->color = 0x00c4a000;
                    break;
                } // dark yellow
                case '4':
                {
                    GlobalRenderer->color = 0x00729fcf;
                    break;
                } // dark blue
                case '5':
                {
                    GlobalRenderer->color = 0x0075507b;
                    break;
                } // dark magenta
                case '6':
                {
                    GlobalRenderer->color = 0x0006989a;
                    break;
                } // dark cyan
                case '7':
                {
                    GlobalRenderer->color = 0x00d3d7cf;
                    break;
                } // dark white
                case '8':
                {
                    GlobalRenderer->color = 0x00555753;
                    break;
                } // bright black
                case '9':
                {
                    GlobalRenderer->color = 0x00ef2929;
                    break;
                } // bright red
                case 'A':
                {
                    GlobalRenderer->color = 0x008ae234;
                    break;
                } // bright green
                case 'B':
                {
                    GlobalRenderer->color = 0x00fce94f;
                    break;
                } // bright yellow
                case 'C':
                {
                    GlobalRenderer->color = 0x0032afff;
                    break;
                } // bright blue
                case 'D':
                {
                    GlobalRenderer->color = 0x00ad7fa8;
                    break;
                } // bright magenta
                case 'E':
                {
                    GlobalRenderer->color = 0x0034e2e2;
                    break;
                } // bright cyan
                case 'F':
                {
                    GlobalRenderer->color = 0x00FFFFFF;
                    break;
                } // bright white
                }
            }
            continue;
        }
        GlobalRenderer->PutChar(buf[i]);
    }
    sched_unlock();
    // puts(buf);
    return i;
}

int debug_printf(const char *fmt, ...)
{
    sched_lock();
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    for (int i = 0; buf[i] != 0; i++)
    {
        if (buf[i] == '%' && buf[i + 1] == 'v')
        {
            i += 2;
        }
        outb(0xE9, buf[i]);
    }
    // puts(buf);
    sched_unlock();
    return i;
}