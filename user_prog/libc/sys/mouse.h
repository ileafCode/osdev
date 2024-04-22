
#ifndef __MEDUZAOS_MOUSE
#define __MEDUZAOS_MOUSE

#include <stdint.h>

typedef struct
{
    uint64_t btnData;
    uint64_t x, y;
} MOUSE_PACKET;

// Get mouse info
MOUSE_PACKET* mouseGetPacket();

#endif