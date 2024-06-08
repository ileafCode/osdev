
#include <string.h>

int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
    for (size_t i = 0; i < num; i++)
    {
        if (*(uint8_t *)(ptr1 + i) < *(uint8_t *)(ptr2 + i))
        {
            return -1;
        }

        if (*(uint8_t *)(ptr1 + i) > *(uint8_t *)(ptr2 + i))
        {
            return 1;
        }
    }

    return 0;
}