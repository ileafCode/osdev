
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
    // bool locked;    // Is locked
} __attribute__((packed));

// Initializes the scheduler
void sched_init();

// Prints out all tasks
void sched_printTasks();
// Called every PIT (Programmable Interval Timer) interrupt
void sched_schedule();
// Makes a process
void sched_makeProc(char name[16], void *entry);
// Yields
void sched_yield();
// Deletes a process
void sched_delProc(uint16_t pid);
// Gets the current process ID
uint16_t sched_getCurPID();
// Deletes itself
void sched_delSelf();
