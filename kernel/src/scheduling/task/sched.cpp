
#include "sched.h"
#include "../../stdio/stdio.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"

Scheduler *GlobalScheduler;
task mainTask;

void idle()
{
    while (1)
        printf("Idle");
}

Scheduler::Scheduler()
{
    asm volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(mainTask.regs.cr3)::"%rax");
    asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(mainTask.regs.eflags)::"%rax");
}
Scheduler::~Scheduler()
{
}
void Scheduler::schedule()
{
    // Return if there are no processes
    if (pids == 0) return;

    // If the time of the process did not pass
    if (this->quantum > 0)
    {
        this->quantum--; // Remove one quantum
        return;          // Return, because we didn't need to switch.
    }

    this->quantum = MAX_QUANTUM;
    this->curPID++;

    if (this->curPID >= this->pids)
    {
        this->curPID = 0;
    }

    // Context switch
    if ((this->ticks % 2) == 0)
    {
        // Switch to main task
    }
    
    // Switch to other task
}

void Scheduler::makeProc(char name[16], void *entry)
{
    this->tasks[this->pids].regs.rax = 0;
    this->tasks[this->pids].regs.rbx = 0;
    this->tasks[this->pids].regs.rcx = 0;
    this->tasks[this->pids].regs.rdx = 0;
    this->tasks[this->pids].regs.rsi = 0;
    this->tasks[this->pids].regs.rdi = 0;
    this->tasks[this->pids].regs.eflags = mainTask.regs.eflags;
    this->tasks[this->pids].regs.rip = (uint64_t) entry;
    this->tasks[this->pids].regs.cr3 = mainTask.regs.cr3;
    this->tasks[this->pids].regs.rsp = (uint64_t) malloc(0x1000) + 0x1000;

    this->tasks[this->pids].pid = this->pids;
    this->tasks[this->pids].state = QUEUED;
    for (int i = 0; i < 16; i++)
        this->tasks[this->pids].name[i] = name[i];
    this->pids++;
}

void Scheduler::delProc(uint16_t pid)
{
}

uint16_t Scheduler::getCurPID()
{
    return this->curPID;
}

void Scheduler::delSelf()
{
    this->delProc(this->getCurPID());
}