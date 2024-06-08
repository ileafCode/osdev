#include "kernelUtil.h"
#include "scheduling/pit/pit.h"
#include "stdio/stdio.h"
#include "fs/fat/ff.h"
#include "elf.h"
#include "fs/vfs.h"
#include "cstr.h"
#include "scheduling/task/sched.h"
#include "cmos/cmos.h"

extern "C" void enableSSE();
extern "C" void enableSCE();
extern "C" void switchUser(void *, void *);

FATFS FatFs;

extern "C" void _start(BootInfo *bootInfo)
{
    InitializeKernel(bootInfo);
    vfsMount('A');
    FRESULT resDrv = f_mount(&FatFs, "", 0);
    if (resDrv)
    {
        printf("Drive is not FAT32 formatted! Error code: %d\n", resDrv);
        while (1)
            ;
    }

    enableSSE();
    sched_init();

    asm("sti");

    debug_printf("[%+10.3f]: Run init\n", PIT::TimeSinceBoot);

    FIL *file;
    FRESULT res = f_open(file, "bin/init", FA_READ);
    void *prg;
    if (res)
        printf("[%v9ERROR%vF]: Unable to read file. Error: %d\n", res);
    else
    {
        uint8_t *buffer = (uint8_t *)malloc(f_size(file));
        f_read(file, buffer, f_size(file), NULL);

        prg = elf_load(0, buffer);
        if (prg != NULL)
        {
            sched_makeProc((char *)"init", (void *)prg, 0, 0);
        }
        else
            printf("[%v9ERROR%vF]: File is not of ELF format.\n");

        free(buffer);
        f_close(file);
    }

    debug_printf("[%+10.3f]: Kernel idle\n", PIT::TimeSinceBoot);

    while (1);
}