
#include <string.h>

void *memcpy(void *destination, const void *source, size_t num)
{
    num /= 8;
    uint64_t *dest64 = (uint64_t *)destination;
    uint64_t *source64 = (uint64_t *)source;

    for (size_t i = 0; i < num; i++)
        *(dest64 + i) = *(source64 + i);
    
    return destination;
}