
#include "mouse.h"
#include "../stdio.h"

MOUSE_PACKET* mouseGetPacket()
{
    MOUSE_PACKET* packet;
    asm volatile("int $0x81;");
    asm volatile("movq %%rbx, %0;" : "=r"(packet->x));
    asm volatile("movq %%rcx, %0;" : "=r"(packet->y));
    asm volatile("movq %%rdx, %0;" : "=r"(packet->btnData));
    return packet;
}