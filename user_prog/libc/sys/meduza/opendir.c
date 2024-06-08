
#include <sys/meduza.h>
#include <sys/syscall.h>

int opendir(DIR *dir, const char *path)
{
    struct sysret_t *ret = syscall(SYSCALL_opendir, (uint64_t)dir, (uint64_t)path, 0);
    return (int)ret->ret1;
}