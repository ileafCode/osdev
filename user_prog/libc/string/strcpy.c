
#include <string.h>

char *strcpy(char *destination, const char *source)
{
    int n = strlen(source) + 1;
    for (int i = 0; i < n; i++)
        destination[i] = source[i];
    return destination;
}