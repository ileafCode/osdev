
#pragma once

#define assert(expr) \
    if (!(expr)) \
    { \
        printf("Assertion \"%s\" failed at line %d in file \"%s\"\n", #expr, __LINE__, __FILE__); \
        asm("cli"); \
        while(1); \
    }