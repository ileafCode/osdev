
#include <sys/meduza.h>
#include <sys/syscall.h>

int readdir(DIR *dir, FILINFO *finfo)
{
    struct sysret_t *ret = syscall(SYSCALL_readdir, (uint64_t)dir, (uint64_t)finfo, 0);
    return (int)ret->ret1;
}