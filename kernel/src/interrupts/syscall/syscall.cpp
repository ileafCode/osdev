
#include "syscall.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"
#include "../../userinput/mouse.h"
#include "../../userinput/keyboard.h"
#include "../../BasicRenderer.h"
#include "../../fs/MeduzaFS.h"

__attribute__((interrupt)) void Syscall0(interrupt_frame *frame)
{
    uint64_t rbx;
    asm volatile("movq %%rbx, %0;" : "=r"(rbx));
    char chr = (char)rbx;
    printf("%c", chr);
}

__attribute__((interrupt)) void Syscall1(interrupt_frame *frame)
{
    asm volatile("movq %0, %%rbx;" : : "r"(MousePosition.X));
    asm volatile("movq %0, %%rcx;" : : "r"(MousePosition.Y));
    asm volatile("movq %0, %%rdx;" : : "r"(mouseButtonData));
}

__attribute__((interrupt)) void Syscall2(interrupt_frame *frame)
{
    uint64_t type; // Type (malloc or free)
    asm volatile("movq %%rbx, %0;" : "=r"(type));

    switch (type)
    {
    case 0:
    {
        uint64_t rcx; // Size
        asm volatile("movq %%rcx, %0;" : "=r"(rcx));

        void *ptr = malloc(rcx);
        asm volatile("movq %0, %%rbx;" : : "r"(ptr));
        break;
    }
    case 1:
    {
        void *ptr; // Pointer to memory
        asm volatile("movq %%rcx, %0;" : "=r"(ptr));
        free(ptr);
        break;
    }
    default:
        printf("Unknown allocation type in syscall 0x82\n");
        while(1);
        break;
    }
}

__attribute__((interrupt)) void Syscall3(interrupt_frame *frame)
{
    // Get framebuffer data struct
    asm volatile("movq %0, %%rbx" : : "r"(GlobalRenderer->TargetFramebuffer));
}

__attribute__((interrupt)) void Syscall4(interrupt_frame *frame)
{
    uint64_t type; // Type
    asm volatile("movq %%rbx, %0;" : "=r"(type));

    switch (type)
    {
    case 0:
    {
        uint64_t sc = (uint64_t)KeyboardGetScancode();
        asm volatile("movq %0, %%rbx;" : : "r"(sc));
        break;
    }
    case 1:
    {
        char *fileName;
        asm volatile("movq %%rcx, %0;" : "=r"(fileName));
        FILE *file = mfsOpenFile(fileName);
        asm volatile("movq %0, %%rbx;" : : "r"(file));
        break;
    }
    }
}
