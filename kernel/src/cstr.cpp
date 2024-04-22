#include "cstr.h"
#include "memory/heap.h"

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

int strncmp(const char *s1, const char *s2, int c)
{
    int result = 0;

    while (c)
    {
        result = *s1 - *s2++;

        if ((result != 0) || (*s1++ == 0))
        {
            break;
        }

        c--;
    }

    return result;
}

char *strstr(const char *in, const char *str)
{
    char c;
    uint32_t len;

    c = *str++;
    if (!c)
        return (char *)in;

    len = strlen(str);
    do
    {
        char sc;

        do
        {
            sc = *in++;
            if (!sc)
                return (char *)0;
        } while (sc != c);
    } while (strncmp(in, str, len) != 0);

    return (char *)(in - 1);
}

char *strsep(char **stringp, const char *delim)
{
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;)
    {
        c = *s++;
        spanp = delim;
        do
        {
            if ((sc = *spanp++) == c)
            {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

char *strdup(const char *src)
{
    int len = strlen(src) + 1;
    char *dst = (char *)malloc(len);
    memcpy(dst, src, len);
    return dst;
}

int memcmp(const void *aptr, const void *bptr, size_t size)
{
    const unsigned char *a = (const unsigned char *)aptr;
    const unsigned char *b = (const unsigned char *)bptr;
    for (size_t i = 0; i < size; i++)
    {
        if (a[i] < b[i])
            return -1;
        else if (b[i] < a[i])
            return 1;
    }
    return 0;
}

void *memcpy(void *__restrict b, const void *__restrict a, size_t n)
{
    char *s1 = (char *)b;
    const char *s2 = (const char *)a;
    for (; 0 < n; --n)
        *s1++ = *s2++;
    return b;
}

void *memmove(void *dstptr, const void *srcptr, size_t size)
{
    unsigned char *dst = (unsigned char *)dstptr;
    const unsigned char *src = (const unsigned char *)srcptr;
    if (dst < src)
    {
        for (size_t i = 0; i < size; i++)
            dst[i] = src[i];
    }
    else
    {
        for (size_t i = size; i != 0; i--)
            dst[i - 1] = src[i - 1];
    }
    return dstptr;
}

void *memset(void *bufptr, int value, size_t size)
{
    unsigned char *buf = (unsigned char *)bufptr;
    for (size_t i = 0; i < size; i++)
        buf[i] = (unsigned char)value;
    return bufptr;
}

void *memset32(void *bufptr, int value, size_t size)
{
    unsigned int *buf = (unsigned int *)bufptr;
    for (size_t i = 0; i < size; i++)
        buf[i] = (unsigned int)value;
    return bufptr;
}

int strint(const char *str)
{
    int sign = 1, base = 0, i = 0;

    while (str[i] == ' ')
        i++;

    if (str[i] == '-' || str[i] == '+')
        sign = 1 - 2 * (str[i++] == '-');

    while (str[i] >= '0' && str[i] <= '9')
    {
        if (base > INT_MAX / 10 || (base == INT_MAX / 10 && str[i] - '0' > 7))
        {
            if (sign == 1)
                return INT_MAX;
            else
                return INT_MIN;
        }
        base = 10 * base + (str[i++] - '0');
    }
    return base * sign;
}

void strcpy(char *str1, char *str2)
{
    int n = strlen(str2);
    for (int i = 0; i < n; i++)
        str1[i] = str2[i];
}

unsigned int is_delim(char c, char *delim)
{
    while (*delim != '\0')
    {
        if (c == *delim)
            return 1;
        delim++;
    }
    return 0;
}

char *strtok(char *srcString, char *delim)
{
    static char *backup_string; // start of the next search
    if (!srcString)
    {
        srcString = backup_string;
    }
    if (!srcString)
    {
        // user is bad user
        return NULL;
    }
    // handle beginning of the string containing delims
    while (1)
    {
        if (is_delim(*srcString, delim))
        {
            srcString++;
            continue;
        }
        if (*srcString == '\0')
        {
            // we've reached the end of the string
            return NULL;
        }
        break;
    }
    char *ret = srcString;
    while (1)
    {
        if (*srcString == '\0')
        {
            /*end of the input string and
            next exec will return NULL*/
            backup_string = srcString;
            return ret;
        }
        if (is_delim(*srcString, delim))
        {
            *srcString = '\0';
            backup_string = srcString + 1;
            return ret;
        }
        srcString++;
    }
}

uint8_t *strsplit(uint8_t *str, uint8_t delim)
{
    int i = 0;

    while (str[i])
    {
        if (str[i] == delim)
        {
            str[i] = 0;
        }
        i++;
    }

    return str + (i + 1);
}

char *strlower(char *str)
{
    int i = 0;

    while (str[i] != 0)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + 32;
        }
        i++;
    }
    return str;
}

char *strupper(char *str)
{
    int i = 0;

    while (str[i] != 0)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] = str[i] - 32;
        i++;
    }
    return str;
}

int strcmp(const char *str1, const char *str2)
{
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);

    if (str1_len != str2_len)
        return 0;

    for (int i = 0; i < str1_len; i++)
        if (str1[i] != str2[i])
            return 0;

    return 1;
}

void strshf(char *str, int start, int end, int shift)
{
    int length = strlen(str);

    if (start < 0 || end >= length || start > end)
    {
        // printf("Invalid input range\n");
        return;
    }

    int shiftAmount = shift % (end - start + 1);

    // Perform the shift
    for (int i = end; i >= start; i--)
    {
        if (i + shiftAmount <= length)
            str[i + shiftAmount] = str[i];
    }
}

char *strcat(char *destination, const char *source)
{
    char *ptr = destination + strlen(destination);

    while (*source != '\0')
        *ptr++ = *source++;

    *ptr = '\0';

    return destination;
}

int strfind(char *string, char c)
{
    int ret = 0;
    for (int i = 0; i < strlen(string); i++)
        if (*(string + i) == c)
            ret++;

    return ret;
}

char uintTo_StringOutput[128];
const char *to_string(uint64_t value)
{
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        uintTo_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    uintTo_StringOutput[size - index] = remainder + '0';
    uintTo_StringOutput[size + 1] = 0;
    return uintTo_StringOutput;
}

char hexTo_StringOutput[128];
const char *to_hstring(uint64_t value)
{
    uint64_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput[size + 1] = 0;
    return hexTo_StringOutput;
}

char hexTo_StringOutput32[128];
const char *to_hstring(uint32_t value)
{
    uint32_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput32[size + 1] = 0;
    return hexTo_StringOutput32;
}

char hexTo_StringOutput16[128];
const char *to_hstring(uint16_t value)
{
    uint16_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput16[size + 1] = 0;
    return hexTo_StringOutput16;
}

char hexTo_StringOutput8[128];
const char *to_hstring(uint8_t value)
{
    uint8_t *valPtr = &value;
    uint8_t *ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t *)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        hexTo_StringOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        hexTo_StringOutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    hexTo_StringOutput8[size + 1] = 0;
    return hexTo_StringOutput8;
}

char intTo_StringOutput[128];
const char *to_string(int64_t value)
{
    uint8_t isNegative = 0;

    if (value < 0)
    {
        isNegative = 1;
        value *= -1;
        intTo_StringOutput[0] = '-';
    }

    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        intTo_StringOutput[isNegative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    intTo_StringOutput[isNegative + size - index] = remainder + '0';
    intTo_StringOutput[isNegative + size + 1] = 0;
    return intTo_StringOutput;
}

char doubleTo_StringOutput[128];
const char *to_string(double value, uint8_t decimalPlaces)
{
    if (decimalPlaces > 20)
        decimalPlaces = 20;

    char *intPtr = (char *)to_string((int64_t)value);
    char *doublePtr = doubleTo_StringOutput;

    if (value < 0)
    {
        value *= -1;
    }

    while (*intPtr != 0)
    {
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for (uint8_t i = 0; i < decimalPlaces; i++)
    {
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;
    return doubleTo_StringOutput;
}

const char *to_string(double value)
{
    return to_string(value, 2);
}
