#include "kernelUtil.h"
#include "scheduling/pit/pit.h"
#include "stdio/stdio.h"
// #include "fs/MeduzaFS.h"
// #include "fs/fat.h"

#include "fs/fat/ff.h"

#include "elf.h"
#include "fs/vfs.h"
#include "cstr.h"
#include "scheduling/task/sched.h"
#include "MeduzaWM/wm.h"
#include "cmos/cmos.h"

FRESULT list_dir(const char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

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
                uint32_t year = ((fno.fdate >> 9) & 0x7F) + 1980;
                uint8_t month = ((fno.fdate >> 5) & 0x0F);
                uint8_t day = ((fno.fdate) & 0x1F);
                printf("%d/%d/%d <DIR> %s\n", day, month, year, fno.fname);
                ndir++;
            }
            else
            {
                uint32_t year = ((fno.fdate >> 9) & 0x7F) + 1980;
                uint8_t month = ((fno.fdate >> 5) & 0x0F);
                uint8_t day = ((fno.fdate) & 0x1F);
                printf("%d/%d/%d <FILE> %s\n", day, month, year, fno.fname);
                nfile++;
            }
        }
        f_closedir(&dir);
        printf("%d dirs, %d files.\n", ndir, nfile);
    }
    else
    {
        printf("Failed to open \"%s\". (%u)\n", path, res);
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
        list_dir(".");
        // mfsListCurDir();
    }
    else if (strcmp((char *)command, "cd"))
    {
        FRESULT res = f_chdir(args[0]);
        // printf("%d\n", res);
        if (res)
            printf("[%o9ERROR%oF]: Unable to change to directory %s\n", args[0]);
        // bool bad = mfsChgDir(args[0]);
        // if (bad)
        //     printf("[%o9ERROR%oF]: Unable to change to directory %s\n", args[0]);
    }
    else if (strcmp((char *)command, "run"))
    {
        /*FILE *file = mfsOpenFile(args[0]);
        if (file != NULL)
        {
            void *prg = elf_load(file->filename, file->data);
            if (prg != NULL)
            {
                GlobalScheduler->makeProc("Process", (void *)prg);
            }
            else
                printf("[%o9ERROR%oF]: %s is not of ELF format.\n", args[0]);
            mfsCloseFile(file);
        }
        else
            printf("[%o9ERROR%oF]: No file found named %s\n", args[0]);*/
    }
    else if (strcmp((char *)command, "ts"))
    {
        // GlobalScheduler->printTasks();
    }
    else if (strcmp((char *)command, "touch"))
    {
        printf("Fname: %s\n", args[0]);
        FIL *file;
        FRESULT res = f_open(file, args[0], FA_CREATE_NEW);
        //if (res)
        //    printf("[%o9ERROR%oF]: Unable to make file\n", args[0]);
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
        printf("[%o9ERROR%oF]: No command named \"%s\"\n", command);
    }
}

static inline void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    __asm__ volatile("cpuid"
                     : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                     : "0"(reg));
}

extern "C" void enableSSE();

void test()
{
    window_t *win = GlobalWM->makeWindow("Hello, world!", 640, 480);
    while (1)
    {
        for (int i = 0; i < win->width * win->height; i++)
            *(win->pixels + i) = i * PIT::TimeSinceBoot;
        if (win->sc == 0x01)
            break;
    }
    GlobalWM->deleteWindow(win);
    sched_delSelf();
    while (1)
        ;
}

FATFS FatFs;

extern "C" void _start(BootInfo *bootInfo)
{
    InitializeKernel(bootInfo);

    vfsMount('A');

    FRESULT resDrv = f_mount(&FatFs, "", 1);
    if (resDrv)
    {
        printf("Drive is not FAT32 formatted! Error code: %d\n", resDrv);
        while (1)
            ;
    }

    // mfsInit();
    enableSSE();

    // ASCII art
    printf(" __       __                  __                                 ______    ______  \n\
/  \\     /  |                /  |                               /      \\  /      \\ \n\
$$  \\   /$$ |  ______    ____$$ | __    __  ________   ______  /$$$$$$  |/$$$$$$  | \n\
$$$  \\ /$$$ | /      \\  /    $$ |/  |  /  |/        | /      \\ $$ |  $$ |$$ \\__$$/ \n\
$$$$  /$$$$ |/$$$$$$  |/$$$$$$$ |$$ |  $$ |$$$$$$$$/  $$$$$$  |$$ |  $$ |$$      \\ \n\
$$ $$ $$/$$ |$$    $$ |$$ |  $$ |$$ |  $$ |  /  $$/   /    $$ |$$ |  $$ | $$$$$$  |\n\
$$ |$$$/ $$ |$$$$$$$$/ $$ \\__$$ |$$ \\__$$ | /$$$$/__ /$$$$$$$ |$$ \\__$$ |/  \\__$$ |\n\
$$ | $/  $$ |$$       |$$    $$ |$$    $$/ /$$      |$$    $$ |$$    $$/ $$    $$/ \n\
$$/      $$/  $$$$$$$/  $$$$$$$/  $$$$$$/  $$$$$$$$/  $$$$$$$/  $$$$$$/   $$$$$$/ \n");

    GlobalRenderer->EnableSecondBuffer();
    GlobalWM = new WindowManager();
    sched_init();

    asm("sti");

    /*GlobalScheduler->makeProc("Test", (void *)test);
    FILE *file = mfsOpenFile("wintst.o");
    if (file != NULL)
    {
        void *prg = (void *)elf_load(file->filename, file->data);
        if (prg != NULL)
        {
            GlobalScheduler->makeProc("Test2", prg);
            GlobalScheduler->makeProc("Test3", prg);
        }
    }
    mfsCloseFile(file);*/

    // FIL *file;
    // FRESULT res = f_open(file, "hello.txt", FA_CREATE_NEW);

    //FRESULT dirRes = f_mkdir("./dirTest");
    // printf("%d\n", dirRes);

    char *cwd = (char *)malloc(100);

    FIL *file;
    FRESULT res = f_open(file, "hello.txt", FA_CREATE_NEW);
    while (true)
    {
        f_getcwd(cwd, 100);
        printf("%oClocalhost%oF@%oDadmin%oF [%s]: ", cwd);
        char *parseStr = KeyboardGetStr();
        //printf("%s\n", parseStr);
        parse(parseStr);
        // GlobalWM->draw();
        // GlobalWM->update();
    }
}