
#include "sched.h"
#include "../../stdio/stdio.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"
#include "../../MeduzaWM/wm.h"

int quantum = MAX_QUANTUM;
// How many processes are there
int pids = 1;
// Current process
uint16_t curPID = 0;
// Previous process
uint16_t prevPID = 0;
// Ticks since boot
uint64_t ticks = 0;
// All the tasks
task tasks[MAX_PROCESS];
// task mainTask;

extern "C" void switchTask(registers *, registers *);

void idle1()
{
    while (1)
        ; // printf(" 1");
}

void sched_init()
{
    for (int i = 0; i < 16; i++)
        tasks[0].name[i] = "Kernel"[i];

    asm volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(tasks[0].regs.cr3)::"%rax");
    asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(tasks[0].regs.eflags)::"%rax");
}

void sched_printTasks()
{
    printf("[Name] : [State] : [PID] : [ADDR] : %d/%d tasks are running\n", pids, MAX_PROCESS);
    for (int i = 0; i < pids; i++)
    {
        task curTask = tasks[i];
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

void sched_schedule()
{
    // Return if there are no processes
    if (pids == 0)
        return;

    // If the time of the process did not pass
    if (quantum > 0)
    {
        quantum--; // Remove one quantum
        return;          // Return, because we didn't need to switch.
    }

    // Reset the quantum
    quantum = MAX_QUANTUM;

    prevPID = curPID;
    curPID++;

    if (curPID >= pids)
    {
        curPID = 0;
    }

    tasks[prevPID].state = QUEUED;
    tasks[curPID].state = RUNNING;

    // printTasks();
    // printf("\n");
    // printf("Switch from PID %d to PID %d\n", this->prevPID, this->curPID);
    //  this->prevPID = this->curPID;

    // Context switch
    switchTask(&tasks[prevPID].regs, &tasks[curPID].regs);
}

void sched_makeProc(char name[16], void *entry)
{
    tasks[pids].regs.rax = 0;
    tasks[pids].regs.rbx = 0;
    tasks[pids].regs.rcx = 0;
    tasks[pids].regs.rdx = 0;
    tasks[pids].regs.rsi = 0;
    tasks[pids].regs.rdi = 0;
    tasks[pids].regs.eflags = tasks[0].regs.eflags;
    tasks[pids].regs.rip = (uint64_t)entry;
    tasks[pids].regs.cr3 = tasks[0].regs.cr3;
    tasks[pids].origStackAddr = (uint64_t)malloc(0x1000);
    tasks[pids].regs.rsp = tasks[pids].origStackAddr + 0x1000;

    tasks[pids].pid = pids;
    tasks[pids].state = QUEUED;

    for (int i = 0; i < 16; i++)
        tasks[pids].name[i] = name[i];

    pids++;
}

void sched_yield()
{
    switchTask(&tasks[curPID].regs, &tasks[curPID + 1].regs);
}

void sched_delProc(uint16_t pid)
{
    free((void *)(tasks[pid].origStackAddr));

    for (int i = pid; i < pids; i++)
    {
        tasks[i] = tasks[i + 1];
        tasks[i].pid--;
    }

    pids--;
}

uint16_t sched_getCurPID()
{
    return curPID;
}

void sched_delSelf()
{
    sched_delProc(sched_getCurPID());
    while (1)
        ;
}