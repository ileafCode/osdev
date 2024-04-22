
#include "sched.h"
#include "../../stdio/stdio.h"
#include "../../paging/PageFrameAllocator.h"

Scheduler *GlobalScheduler;

task mainTask;

void idle()
{
    while(1) printf("Idle");
}

Scheduler::Scheduler()
{
    asm volatile("movq %%cr3, %%rax; movq %%rax, %0;":"=m"(mainTask.regs.cr3)::"%rax");
    asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;":"=m"(mainTask.regs.eflags)::"%rax");
}
Scheduler::~Scheduler()
{
}
void Scheduler::schedule()
{
    // If the time of the process did not pass
    if (this->quantum > 0)
    {
        this->quantum--; // Remove one quantum
        return;          // Return, because we didn't need to switch.
    }

    this->quantum = MAX_QUANTUM;
    curPID++;

    if (curPID >= MAX_PROCESS)
    {
        curPID = 0;
    }

    // Context switch
    
}
void Scheduler::makeProc(char name[16], void *entry)
{

}
void Scheduler::delProc(uint16_t pid)
{
}