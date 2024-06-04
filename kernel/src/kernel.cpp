#include "kernelUtil.h"
#include "scheduling/pit/pit.h"
#include "stdio/stdio.h"
#include "fs/fat/ff.h"
#include "elf.h"
#include "fs/vfs.h"
#include "cstr.h"
#include "scheduling/task/sched.h"
#include "cmos/cmos.h"

FRESULT list_dir(const char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    printf("%-10s Time Type  %-25s Size (bytes)\n", "Date", "Name");

    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
                break;
            if (fno.fattrib & AM_DIR)
            {
                int year = ((fno.fdate >> 9) & 0x7F) + 1980;
                int month = ((fno.fdate >> 5) & 0x0F);
                int day = ((fno.fdate) & 0x1F);

                int hour = ((fno.ftime >> 11));
                int minute = ((fno.ftime >> 5) & 0x1F);

                printf("%02d/%02d/%d %d:%02d <DIR> %-25s --\n", day, month, year, hour, minute, fno.fname);
                ndir++;
            }
            else
            {
                int year = ((fno.fdate >> 9) & 0x7F) + 1980;
                int month = ((fno.fdate >> 5) & 0x0F);
                int day = ((fno.fdate) & 0x1F);

                int hour = ((fno.ftime >> 11));
                int minute = ((fno.ftime >> 5) & 0x1F);

                printf("%02d/%02d/%d %d:%02d <FIL> %-25s %d\n", day, month, year, hour, minute, fno.fname, fno.fsize);
                nfile++;
            }
        }
        f_closedir(&dir);
        printf("%d dirs, %d files.\n", ndir, nfile);
    }
    else
    {
        printf("[%v9ERROR%vF]: Failed to open \"%s\". (%u)\n", path, res);
    }
    return res;
}

void parse(char *str)
{
    char *command = (char *)strtok(str, " ");
    char *arg = (char *)strsplit((uint8_t *)str, ' ');
    strlower((char *)command);

    char *tok = strtok(arg, " ");
    int arg_i = 0;

    char *args[10];

    while (tok != NULL)
    {
        args[arg_i] = tok;
        tok = strtok(NULL, " ");
        arg_i++;
    }

    // printf("%s %s\n", command, arg);

    if (strcmp((char *)command, "echo"))
    {
        for (int i = 0; i < arg_i; i++)
            printf("%s ", args[i]);
        printf("\n");
    }
    else if (strcmp((char *)command, "clear"))
    {
        GlobalRenderer->Clear();
    }
    else if (strcmp((char *)command, "ls"))
    {
        // if (strcmp(args[0], "-l"))
        //     list_dir(".");
        // else
        list_dir(".");
    }
    else if (strcmp((char *)command, "cd"))
    {
        FRESULT res = f_chdir(args[0]);
        if (res)
            printf("[%v9ERROR%vF]: Unable to change to directory %s\n", args[0]);
    }
    else if (strcmp((char *)command, "run"))
    {
        FIL *file;
        FRESULT res = f_open(file, args[0], FA_READ);
        if (res)
            printf("[%v9ERROR%vF]: Unable to read file. Error: %d\n", res);
        else
        {
            uint8_t *buffer = (uint8_t *)malloc(f_size(file));
            f_read(file, buffer, f_size(file), NULL);

            void *prg = elf_load(args[0], buffer);
            if (prg != NULL)
            {
                sched_makeProc(args[0], (void *)prg, arg_i, args);
            }
            else
                printf("[%v9ERROR%vF]: File is not of ELF format.\n");

            free(buffer);
            f_close(file);
        }
    }
    else if (strcmp((char *)command, "ts"))
    {
        sched_printTasks();
    }
    else if (strcmp((char *)command, "mkdir"))
    {
        FRESULT res = f_mkdir(args[0]);
        if (res)
            printf("[%v9ERROR%vF]: Unable to make directory\n");
    }
    else if (strcmp((char *)command, "touch"))
    {
        FIL *file;
        FRESULT res = f_open(file, args[0], FA_CREATE_NEW);
        if (res)
            printf("[%v9ERROR%vF]: Unable to make file. (%d)\n", res);
        else
            f_close(file);
    }
    else if (strcmp((char *)command, "read"))
    {
        FIL *file;
        FRESULT res = f_open(file, args[0], FA_READ);
        if (res)
            printf("[%v9ERROR%vF]: Unable to read file. Error: %d\n", res);
        else
        {
            uint8_t *buffer = (uint8_t *)malloc(f_size(file));
            f_read(file, buffer, f_size(file), NULL);

            for (int i = 0; i < f_size(file); i++)
                printf("%c", buffer[i]);
            printf("\n");

            free(buffer);
            f_close(file);
        }
    }
    else if (strcmp((char *)command, "help"))
    {
        printf("=== Help Menu ===\n");
        printf("echo <args> - Prints out the arguments\n");
        printf("clear - Clears the screen\n");
        printf("ls - Lists the files and directories in the current directory\n");
        printf("cd <dirpath> - Changes the directory\n");
        printf("run <filepath> - Runs an ELF file\n");
        printf("ts - Prints all tasks and their names, states and PID\n");
        printf("help - Prints this menu\n");
    }
    else if (strcmp((char *)command, (char *)0))
        ;
    else
    {
        printf("[%v9ERROR%vF]: No command named \"%s\"\n", command);
    }
}

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
    enableSCE();
    sched_init();

    asm("sti");

    debug_printf("[%+10.3f]: Run init\n", PIT::TimeSinceBoot);

    FIL *file;
    FRESULT res = f_open(file, "init", FA_READ);
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
            sched_makeProc("init", (void *)prg, 0, 0);
        }
        else
            printf("[%v9ERROR%vF]: File is not of ELF format.\n");

        free(buffer);
        f_close(file);
    }

    debug_printf("[%+10.3f]: Kernel idle\n", PIT::TimeSinceBoot);

    while (1);
        
    
    char *cwd = (char *)malloc(100);
    while (1)
    {
        f_getcwd(cwd, 100);
        printf("%vClocalhost%vF@%vAadmin%vF [%s]: ", cwd);
        char *parseStr = KeyboardGetStr();
        parse(parseStr);

        // GlobalWM->draw();
        // GlobalWM->update();
    }
}