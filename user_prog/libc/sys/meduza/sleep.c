
#include <sys/meduza.h>
#include <sys/syscall.h>

void sleep(uint64_t ms)
{
    struct sysret_t *ret = syscall(SYSCALL_sleepFunc, 0, 0, 0);
    void (*sleep_ptr)(uint64_t) = (void (*)(uint64_t))ret->ret1;
    sleep_ptr(ms);
}