
#include "sched.h"
#include "../../stdio/stdio.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"
#include "../../fs/fat/ff.h"

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

// Is locked
bool locked = false;

extern "C" void switchTask(registers *, registers *);

void sched_lock()
{
    locked = true;
}

void sched_unlock()
{
    locked = false;
}

int enabled = false;

void sched_init()
{
    for (int i = 0; i < 16; i++)
        tasks[0].name[i] = "Kernel"[i];

    //asm volatile("movq %%cr3, %%rax; movq %%rax, %0;" : "=m"(tasks[0].regs.cr3)::"%rax");
    asm volatile("pushfq; movq (%%rsp), %%rax; movq %%rax, %0; popfq;" : "=m"(tasks[0].regs.eflags)::"%rax");
    enabled = true;
}

void sched_printTasks()
{
    if (!enabled) return;
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

int times = 0;

void sched_schedule(int chgTask)
{
    if (!enabled) return;
    if (locked) return;
    if (pids == 0) return;

    // If the time of the process did not pass
    if (quantum > 0)
    {
        quantum--; // Remove one quantum
        return;    // Return, because we didn't need to switch.
    }

    // Reset the quantum
    quantum = MAX_QUANTUM;

    prevPID = curPID;
    curPID++;

    if (curPID >= pids)
        curPID = 0;
    
    f_getcwd(tasks[prevPID].cwd, 100);
    //debug_printf("Times: %d\n", times);
    f_chdir(tasks[curPID].cwd);

    tasks[prevPID].state = QUEUED;
    tasks[curPID].state = RUNNING;

    // Context switch
    times++;
    if (chgTask)
        switchTask(&tasks[prevPID].regs, &tasks[curPID].regs);
}

void sched_makeProc(char name[16], void *entry, int argc, char **argv)
{
    tasks[pids].regs.rax = 0;
    tasks[pids].regs.rbx = 0;
    tasks[pids].regs.rcx = argc;
    tasks[pids].regs.rdx = (uint64_t)argv;
    tasks[pids].regs.rsi = 0;
    tasks[pids].regs.rdi = 0;
    tasks[pids].regs.r8 = 0;
    tasks[pids].regs.eflags = tasks[0].regs.eflags;
    tasks[pids].regs.rip = (uint64_t)entry;
    tasks[pids].origStackAddr = (uint64_t)malloc(0x1000);
    tasks[pids].regs.rsp = tasks[pids].origStackAddr + 0x1000;

    tasks[pids].pid = pids;
    tasks[pids].state = QUEUED;
    tasks[pids].cwd = (char *)malloc(100);
    tasks[pids].cwd[0] = '/';
    tasks[pids].cwd[1] = '\0';

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
    sched_lock();
    free(tasks[pid].cwd);
    free((void *)(tasks[pid].origStackAddr));

    for (int i = pid; i < pids; i++)
    {
        tasks[i] = tasks[i + 1];
        tasks[i].pid--;
    }

    pids--;
    sched_unlock();
}

uint16_t sched_getCurPID()
{
    return curPID;
}

void sched_delSelf()
{
    sched_delProc(sched_getCurPID());
}