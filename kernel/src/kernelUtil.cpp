#include "kernelUtil.h"
#include "gdt/gdt.h"
#include "interrupts/IDT.h"
#include "interrupts/interrupts.h"
#include "interrupts/syscall/syscall.h"
#include "IO.h"
#include "memory/heap.h"
#include "ahci/ahci.h"
#include "stdio/stdio.h"
#include "fs/vfs.h"
#include "apic/apic.h"
#include "scheduling/task/sched.h"

KernelInfo kernelInfo;

void PrepareMemory(BootInfo *bootInfo)
{
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescSize;

    GlobalAllocator = PageFrameAllocator();
    GlobalAllocator.ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;

    GlobalAllocator.LockPages(&_KernelStart, kernelPages);

    PageTable *PML4 = (PageTable *)GlobalAllocator.RequestPage();
    memset(PML4, 0, 0x1000);

    g_PageTableManager = PageTableManager(PML4);

    for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescSize); t += 0x1000)
        g_PageTableManager.MapMemory((void *)t, (void *)t);

    uint64_t fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
    GlobalAllocator.LockPages((void *)fbBase, fbSize / 0x1000 + 1);
    for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096)
        g_PageTableManager.MapMemory((void *)t, (void *)t);

    asm("mov %0, %%cr3" : : "r"(PML4));

    kernelInfo.pageTableManager = &g_PageTableManager;
}

IDTR idtr;
void SetIDTGate(void *handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector)
{
    IDTDescEntry *interrupt = (IDTDescEntry *)(idtr.Offset + entryOffset * sizeof(IDTDescEntry));
    interrupt->SetOffset((uint64_t)handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}

void PrepareInterrupts()
{
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)GlobalAllocator.RequestPage();

    SetIDTGate((void *)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)GPFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)PITInt_Handler, 0x20, IDT_TA_InterruptGate, 0x08);

    SetIDTGate((void *)Syscall0, 0x80, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)Syscall1, 0x81, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)Syscall2, 0x82, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)Syscall3, 0x83, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)Syscall4, 0x84, IDT_TA_InterruptGate, 0x08);
    SetIDTGate((void *)Syscall5, 0x85, IDT_TA_InterruptGate, 0x08);

    asm("lidt %0" : : "m"(idtr));

    RemapPIC();

    printf("[%oBIDT%oF]: IDT initialized\n");
}

void PrepareACPI(BootInfo *bootInfo)
{
    printf("[%o6ACPI%oF]: Finding XSDT table...\n");
    ACPI::SDTHeader *xsdt = (ACPI::SDTHeader *)(bootInfo->rsdp->XSDTAddress);
    printf("[%o6ACPI%oF]: Finding MCFG table...\n");
    ACPI::MCFGHeader *mcfg = (ACPI::MCFGHeader *)ACPI::FindTable(xsdt, (char *)"MCFG");
    printf("[%o6ACPI%oF]: Enumerating through PCI...\n");
    PCI::EnumeratePCI(mcfg);
    //apicInit((uint8_t*)bootInfo->rsdp->XSDTAddress);
}
extern AHCI::AHCIDriver *g_ahciDriver;

BasicRenderer r = BasicRenderer(NULL, NULL);
KernelInfo InitializeKernel(BootInfo *bootInfo)
{
    r = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_Font);
    r.PSF2_Font = bootInfo->psf2_Font;
    GlobalRenderer = &r;

    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    PrepareMemory(bootInfo);

    memset(bootInfo->framebuffer->BaseAddress, 0, bootInfo->framebuffer->BufferSize);

    printf("[%oCGDT%oF]: GDT initialized\n");
    printf("[%oEMEM%oF]: Paging initialized\n");

    InitializeHeap((void *)0x0000100000000000, 0x10000);

    PrepareInterrupts();

    InitPS2Mouse();
    printf("[%o5PS2%oF]: Mouse initialized\n");

    PrepareKeyboard();
    printf("[%o5PS2%oF]: Keyboard initialized\n");

    outb(PIC1_DATA, 0b11111000);
    outb(PIC2_DATA, 0b11101111);

    //asm("sti");

    PrepareACPI(bootInfo);

    return kernelInfo;
}