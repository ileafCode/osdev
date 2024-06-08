
#include <sys/meduza.h>
#include <sys/syscall.h>

int closedir(DIR *dir)
{
    struct sysret_t *ret = syscall(SYSCALL_closedir, (uint64_t)dir, 0, 0);
    return (int)ret->ret1;
}