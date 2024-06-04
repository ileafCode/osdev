
#ifndef __MEDUZAOS_SYSCALL
#define __MEDUZAOS_SYSCALL

#include <stdint.h>

#define SYSCALL_exit         0
#define SYSCALL_makeProc     1
#define SYSCALL_delProc      2
#define SYSCALL_openFile     3
#define SYSCALL_closeFile    4
#define SYSCALL_read         5
#define SYSCALL_write        6
#define SYSCALL_tell         7
#define SYSCALL_seek         8
#define SYSCALL_getsize      9
#define SYSCALL_readScanCode 10
#define SYSCALL_writeConsole 11
#define SYSCALL_malloc       12
#define SYSCALL_free         13

struct sysret_t {
    uint64_t ret1;
    uint64_t ret2;
    uint64_t ret3;
    uint64_t ret4;
} __attribute__((packed));

struct sysret_t *syscall(uint64_t type, uint64_t r9, uint64_t r10, uint64_t r11);

#endif