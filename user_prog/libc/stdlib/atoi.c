
#include <stdlib.h>

#include <stddef.h>
#include <stdint.h>

int atoi(const char *str)
{
    int sign = 1, base = 0, i = 0;

    while (str[i] == ' ')
        i++;

    if (str[i] == '-' || str[i] == '+')
        sign = 1 - 2 * (str[i++] == '-');

    while (str[i] >= '0' && str[i] <= '9')
    {
        if (base > INT32_MAX / 10 || (base == INT32_MAX / 10 && str[i] - '0' > 7))
        {
            if (sign == 1)
                return INT32_MAX;
            else
                return INT32_MIN;
        }
        base = 10 * base + (str[i++] - '0');
    }
    return base * sign;
}