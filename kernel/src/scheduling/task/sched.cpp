
#include "sched.h"
#include "../../stdio/stdio.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"

Scheduler *GlobalScheduler;
// task mainTask;

extern "C" void switchTask(registers *, registers *);

void idle1()
{
    while (1)
        ;//printf(" 1");
}

Scheduler::Scheduler()
{
    for (int i = 0; i < 16; i++)
        this->tasks[0].name[i] = "Kernel"[i];

    asm volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(this->tasks[0].regs.cr3)::"%rax");
    asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(this->tasks[0].regs.eflags)::"%rax");

    this->makeProc("Idle", (void*)idle1);
}
Scheduler::~Scheduler()
{
}

void Scheduler::printTasks()
{
    printf("[Name] : [State] : [PID] : [ADDR] : %d/%d tasks are running\n", this->pids, MAX_PROCESS);
    for (int i = 0; i < this->pids; i++)
    {
        task curTask = this->tasks[i];
        char state[10];
        if (curTask.state == RUNNING)
            for (int i = 0; i < 10; i++)
                state[i] = "Running"[i];
        else if (curTask.state == QUEUED)
            for (int i = 0; i < 10; i++)
                state[i] = "Queued"[i];
        printf("[%s] : [%s] : [%d] : [%x]\n", curTask.name, state, curTask.pid, curTask.regs.rip);
    }
}

void Scheduler::schedule()
{
    // Return if there are no processes
    if (pids == 0)
        return;

    // If the time of the process did not pass
    if (this->quantum > 0)
    {
        this->quantum--; // Remove one quantum
        return;          // Return, because we didn't need to switch.
    }

    // Reset the quantum
    this->quantum = MAX_QUANTUM;

    this->prevPID = this->curPID;
    this->curPID++;

    if (this->curPID >= this->pids)
    {
        this->curPID = 0;
    }

    this->tasks[this->prevPID].state = QUEUED;
    this->tasks[this->curPID].state = RUNNING;

    //printTasks();
    //printf("\n");
    //printf("Switch from PID %d to PID %d\n", this->prevPID, this->curPID);
    // this->prevPID = this->curPID;

    // Context switch
    switchTask(&this->tasks[this->prevPID].regs, &this->tasks[this->curPID].regs);
}

void Scheduler::makeProc(char name[16], void *entry)
{
    this->tasks[this->pids].regs.rax = 0;
    this->tasks[this->pids].regs.rbx = 0;
    this->tasks[this->pids].regs.rcx = 0;
    this->tasks[this->pids].regs.rdx = 0;
    this->tasks[this->pids].regs.rsi = 0;
    this->tasks[this->pids].regs.rdi = 0;
    this->tasks[this->pids].regs.eflags = this->tasks[0].regs.eflags;
    this->tasks[this->pids].regs.rip = (uint64_t)entry;
    this->tasks[this->pids].regs.cr3 = this->tasks[0].regs.cr3;
    this->tasks[this->pids].regs.rsp = (uint64_t)malloc(0x1000) + 0x1000;

    this->tasks[this->pids].pid = this->pids;
    this->tasks[this->pids].state = QUEUED;

    for (int i = 0; i < 16; i++)
        this->tasks[this->pids].name[i] = name[i];

    this->pids++;
}

void Scheduler::yield()
{
    switchTask(&this->tasks[this->curPID].regs, &this->tasks[this->curPID + 1].regs);
}

void Scheduler::delProc(uint16_t pid)
{
    free((void *)(this->tasks[pid].regs.rsp - 0x1000));

    for (int i = pid; i < this->pids; i++)
    {
        this->tasks[i] = this->tasks[i + 1];
    }

    this->pids--;
}

uint16_t Scheduler::getCurPID()
{
    return this->curPID;
}

void Scheduler::delSelf()
{
    this->delProc(this->getCurPID());
}