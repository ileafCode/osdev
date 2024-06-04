#include "syscall.h"

#define SPECIAL_RETADDRESS 0x2000

struct sysret_t *syscall(uint64_t type, uint64_t r9, uint64_t r10, uint64_t r11)
{
    uint64_t saved_r8, saved_r9, saved_r10, saved_r11;

    asm volatile("movq %%r8, %0;"
        "movq %%r9, %1;"
        "movq %%r10, %2;"
        "movq %%r11, %3;"
        : "=r"(saved_r8), "=r"(saved_r9), "=r"(saved_r10), "=r"(saved_r11));

    asm volatile("movq %0, %%r8;"
        "movq %1, %%r9;"
        "movq %2, %%r10;"
        "movq %3, %%r11;"
        :
        : "r"(type), "r"(r9), "r"(r10), "r"(r11)
        : "%r8", "%r9", "%r10", "%r11");
    
    asm volatile("int $0x80");

    asm volatile("movq %0, %%r8;"
        "movq %1, %%r9;"
        "movq %2, %%r10;"
        "movq %3, %%r11;"
        :
        : "r"(saved_r8), "r"(saved_r9), "r"(saved_r10), "r"(saved_r11)
        : "%r8", "%r9", "%r10", "%r11");
    
    return (struct sysret_t *)(SPECIAL_RETADDRESS);
}