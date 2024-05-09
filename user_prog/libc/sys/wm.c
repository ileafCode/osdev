
#include "wm.h"

extern void func();

window_t *wm_requestWindow(char *name, int width, int height)
{
    func();
    asm volatile ("movq %0, %%rbx" : : "r"(name));
    asm volatile ("movq %0, %%rcx" : : "r"((uint64_t)width));
    asm volatile ("movq %0, %%rdx" : : "r"((uint64_t)height));
    asm volatile ("movq %0, %%r8" : : "r"((uint64_t)0));
    asm volatile ("int $0x86");

    uint64_t window;
    asm volatile ("movq %%rbx, %0" : "=r"(window));

    //register uint64_t rbx asm("rbx");
    return (window_t *)window;
}

void wm_closeWindow(window_t *window)
{
    asm volatile ("movq %0, %%rbx; movq $1, %%r8; int $0x86" : : "r"(window));
}