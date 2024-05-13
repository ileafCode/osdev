
#include "apic.h"
#include "../scheduling/pit/pit.h"

uint8_t lapic_ids[256] = {0};
uint8_t numcore = 0;
uint32_t lapic_ptr = 0;
uint32_t ioapic_ptr = 0;

static inline void mmioWrite32(void *p, uint32_t data)
{
    *(volatile uint32_t *)(p) = data;
}

static inline uint32_t mmioRead32(void *p)
{
    return *(volatile uint32_t *)(p);
}

static inline void mmioWrite64(void *p, uint64_t data)
{
    *(volatile uint64_t *)(p) = data;
}

static inline uint64_t mmioRead64(void *p)
{
    return *(volatile uint64_t *)(p);
}

static uint32_t lapicIn(uint32_t reg)
{
    return mmioRead32((void *)lapic_ptr + reg);
}

static void lapicOut(uint32_t reg, uint32_t data)
{
    mmioWrite32((void *)lapic_ptr + reg, data);
}

void lapicSendInit(uint64_t apic_id)
{
    lapicOut(LAPIC_ICRHI, apic_id << ICR_DESTINATION_SHIFT);
    lapicOut(LAPIC_ICRLO, ICR_INIT | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    while (lapicIn(LAPIC_ICRLO) & ICR_SEND_PENDING)
        ;
}

void lapicSendStartup(uint64_t apic_id, uint64_t vector)
{
    lapicOut(LAPIC_ICRHI, apic_id << ICR_DESTINATION_SHIFT);
    lapicOut(LAPIC_ICRLO, vector | ICR_STARTUP | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    while (lapicIn(LAPIC_ICRLO) & ICR_SEND_PENDING)
        ;
}

void apicInit(uint8_t *rsdt)
{
    uint8_t *ptr, *ptr2;
    uint32_t len;

    for (len = *((uint32_t *)(rsdt + 4)), ptr2 = rsdt + 36; ptr2 < rsdt + len; ptr2 += rsdt[0] == 'X' ? 8 : 4)
    {
        ptr = (uint8_t *)(uintptr_t)(rsdt[0] == 'X' ? *((uint32_t *)ptr2) : *((uint32_t *)ptr2));
        if (!memcmp(ptr, "APIC", 4))
        {
            lapic_ptr = (uint32_t)(*((uint32_t *)(ptr + 0x24)));
            ptr2 = ptr + *((uint32_t *)(ptr + 4));

            for (ptr += 44; ptr < ptr2; ptr += ptr[1])
            {
                switch (ptr[0])
                {
                case 0:
                    if (ptr[4] & 1)
                        lapic_ids[numcore++] = ptr[3];
                    break;
                case 1:
                    ioapic_ptr = (uint32_t) * ((uint32_t *)(ptr + 4));
                    break;
                case 5:
                    lapic_ptr = *((uint32_t *)(ptr + 4));
                    break;
                }
            }
            break;
        }
    }

    g_PageTableManager.MapMemory((void *)ioapic_ptr, (void *)ioapic_ptr);
    g_PageTableManager.MapMemory((void *)lapic_ptr, (void *)lapic_ptr);
    //printf("IOAPIC pointer: %x\nLAPIC pointer: %x\nNumber of cores: %d\n", ioapic_ptr, lapic_ptr, numcore);

    lapicOut(LAPIC_TPR, 0);
    lapicOut(LAPIC_DFR, 0xffffffff);
    lapicOut(LAPIC_LDR, 0x01000000);
    lapicOut(LAPIC_SVR, 0x100 | 0xff);
}
