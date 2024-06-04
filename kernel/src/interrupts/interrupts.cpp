#include "interrupts.h"
#include "../stdio/stdio.h"
#include "../panic.h"
#include "../IO.h"
#include "../userinput/keyboard.h"
#include "../scheduling/pit/pit.h"

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame *frame)
{
    if (sched_getCurPID() != 0)
    {
        debug_printf("PAGE_FAULT: PID %d @ 0x%x\n", sched_getCurPID(), frame->rip);
        sched_delSelf();
        sched_schedule();
    }

    asm("cli");
    Panic("PAGE_FAULT in Kernel\n");
    debug_printf("PAGE_FAULT in Kernel\n");
    printf("at %x\n", frame->rip);
    while (true)
        ;
}

__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame *frame)
{
    if (sched_getCurPID() != 0)
    {
        debug_printf("DB_FAULT: PID %d @ 0x%x\n", sched_getCurPID(), frame->rip);
        sched_delSelf();
        sched_schedule();
    }

    asm("cli");
    Panic("DB_FAULT in Kernel\n");
    debug_printf("DB_FAULT in Kernel\n");
    printf("at %x\n", frame->rip);
    while (true)
        ;
}

__attribute__((interrupt)) void GPFault_Handler(interrupt_frame *frame, unsigned long int error)
{
    if (sched_getCurPID() != 0)
    {
        debug_printf("GP_FAULT: PID %d @ 0x%x, %d\n", sched_getCurPID(), frame->rip, error);
        sched_delSelf();
        sched_schedule();
    }
    
    asm("cli");
    Panic("GP_FAULT in Kernel\n");
    debug_printf("GP_FAULT in Kernel, Error: %d\n", error);
    printf("at %x\n", frame->rip);
    while (true)
        ;
}

__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame *frame)
{
    uint8_t scancode = inb(0x60);
    HandleKeyboard(scancode);
    PIC_EndMaster();
}

__attribute__((interrupt)) void MouseInt_Handler(interrupt_frame *frame)
{
    uint8_t mouseData = inb(0x60);
    HandlePS2Mouse(mouseData);
    PIC_EndSlave();
}

__attribute__((interrupt)) void PITInt_Handler(interrupt_frame *frame)
{
    PIT::Tick();
    PIC_EndMaster();
    sched_schedule();
}

void PIC_EndMaster()
{
    outb(PIC1_COMMAND, PIC_EOI);
}

void PIC_EndSlave()
{
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void RemapPIC()
{
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
}