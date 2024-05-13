
#include "syscall.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"
#include "../../userinput/mouse.h"
#include "../../userinput/keyboard.h"
#include "../../BasicRenderer.h"
//#include "../../fs/MeduzaFS.h"
#include "../../scheduling/task/sched.h"
#include "../../MeduzaWM/wm.h"

__attribute__((interrupt)) void Syscall0(interrupt_frame *frame)
{
    asm("cli");
    uint64_t rbx;
    asm volatile("movq %%rbx, %0;" : "=r"(rbx));
    char chr = (char)rbx;
    printf("%c", chr);
    asm("sti");
}

__attribute__((interrupt)) void Syscall1(interrupt_frame *frame)
{
    asm("cli");
    asm volatile("movq %0, %%rbx;" : : "r"(MousePosition.X));
    asm volatile("movq %0, %%rcx;" : : "r"(MousePosition.Y));
    asm volatile("movq %0, %%rdx;" : : "r"(mouseButtonData));
    asm("sti");
}

__attribute__((interrupt)) void Syscall2(interrupt_frame *frame)
{
    asm("cli");
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
        while (1)
            ;
        break;
    }
    asm("sti");
}

__attribute__((interrupt)) void Syscall3(interrupt_frame *frame)
{
    asm("cli");
    // Get framebuffer data struct
    asm volatile("movq %0, %%rbx" : : "r"(GlobalRenderer->TargetFramebuffer));
    asm("sti");
}

__attribute__((interrupt)) void Syscall4(interrupt_frame *frame)
{
    asm("cli");
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
        //FILE *file = mfsOpenFile(fileName);
        //asm volatile("movq %0, %%rbx;" : : "r"(file));
        break;
    }
    }
    asm("sti");
}

__attribute__((interrupt)) void Syscall5(interrupt_frame *frame)
{
    asm("cli");
    sched_delSelf();
    asm("sti");
}

// Returns pointer to window in RBX
__attribute__((interrupt)) void Syscall6(interrupt_frame *frame)
{
    asm("cli");
    uint64_t rax;
    asm volatile("movq %%r8, %0" : "=r"(rax));

    switch (rax)
    {
    case 0:
    {
        uint64_t rbx, rcx, rdx;
        asm volatile("movq %%rbx, %0" : "=r"(rbx));
        asm volatile("movq %%rcx, %0" : "=r"(rcx));
        asm volatile("movq %%rdx, %0" : "=r"(rdx));
        // register uint64_t rbx asm("rbx"); // Window Name
        // register uint64_t rcx asm("rcx"); // Window Width
        // register uint64_t rdx asm("rdx"); // Window Height

        char *winName = (char *)rbx;
        uint16_t winWidth = (uint16_t)rcx;
        uint16_t winHeight = (uint16_t)rdx;
        window_t *window = GlobalWM->makeWindow(winName, winWidth, winHeight);

        printf("Created window at 0x%x with width %d and height %d\n", window, winWidth, winHeight);
        asm volatile("movq %0, %%rbx;" : : "r"(window));
        printf("RBX: %x\n", rbx);
        break;
    }
    case 1:
    {
        uint64_t rbx;
        asm volatile("movq %%rbx, %0" : "=r"(rbx));
        GlobalWM->deleteWindow((window_t *)rbx);
        break;
    }

    //default:
    //    break;
    }

    asm("sti");
}
