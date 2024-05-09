
#pragma once
#include "../../vector/vector.h"
#include <stdint.h>

#define MAX_QUANTUM 0  // Max amount of quanta (quanta means quantum plural)
#define MAX_PROCESS 20 // Maximum amount of processes

// All 64-bit registers
struct registers
{
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp, rip, eflags, cr3;
} __attribute__((packed));

// Task states
enum TaskState
{
    RUNNING = 0,
    QUEUED = 1,
    KILL = 2
};

// Task struct
struct task
{
    uint64_t origStackAddr;
    registers regs; // Registers for each task
    uint16_t pid;   // Process ID
    char name[16];  // Process name
    int state;      // Can be RUNNING, QUEUED or KILL
    //bool locked;    // Is locked
} __attribute__((packed));

// Scheduler class
class Scheduler
{
public:
    // Constructor and destructor
    Scheduler();
    ~Scheduler();
    // Prints out all tasks
    void printTasks();
    // Called every PIT (Programmable Interval Timer) interrupt
    void schedule();
    // Makes a process
    void makeProc(char name[16], void *entry);
    // Yields
    void yield();
    // Deletes a process
    void delProc(uint16_t pid);
    // Gets the current process ID
    uint16_t getCurPID();
    // Deletes itself
    void delSelf();

private:
    // Decides how long a task can be run (ex. 5q = 50ms, 1q = 10ms, etc.)
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
    //Vector<task> tasks;
};

extern Scheduler *GlobalScheduler;
