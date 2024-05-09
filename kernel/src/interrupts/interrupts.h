#pragma once
#include "../BasicRenderer.h"
#include "../scheduling/task/sched.h"
#include "../userinput/mouse.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct interrupt_frame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void GPFault_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void MouseInt_Handler(interrupt_frame *frame);
__attribute__((interrupt)) void PITInt_Handler(interrupt_frame *frame);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();