#include "kernelUtil.h"
#include "scheduling/pit/pit.h"
#include "stdio/stdio.h"
#include "fs/MeduzaFS.h"
#include "fs/fat.h"
#include "elf.h"
#include "fs/vfs.h"
#include "cstr.h"
#include "scheduling/task/sched.h"
#include "MeduzaWM/wm.h"
#include "cmos/cmos.h"

void parse(char *str)
{
    uint8_t *command = (uint8_t *)strtok(str, " ");
    uint8_t *arg = strsplit((uint8_t *)str, ' ');
    strlower((char *)command);

    char *tok = strtok((char *)arg, " ");
    int arg_i = 0;

    char *args[40];

    while (tok != NULL)
    {
        args[arg_i] = tok;
        tok = strtok(NULL, " ");
        arg_i++;
    }

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
        mfsListCurDir();
    }
    else if (strcmp((char *)command, "cd"))
    {
        bool bad = mfsChgDir(args[0]);
        if (bad)
            printf("[%o9ERROR%oF]: Unable to change to directory %s\n", args[0]);
    }
    else if (strcmp((char *)command, "run"))
    {
        FILE *file = mfsOpenFile(args[0]);
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
            printf("[%o9ERROR%oF]: No file found named %s\n", args[0]);
    }
    else if (strcmp((char *)command, "ts"))
    {
        GlobalScheduler->printTasks();
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
    GlobalScheduler->delSelf();
    while (1)
        ;
}

extern "C" void _start(BootInfo *bootInfo)
{
    InitializeKernel(bootInfo);

    vfsMount('A');
    mfsInit();
    GlobalRenderer->EnableSecondBuffer();
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

    GlobalWM = new WindowManager();
    GlobalScheduler = new Scheduler();

    asm("sti");
    GlobalScheduler->makeProc("Test2", (void *)test);
    FILE *file = mfsOpenFile("wintst.o");
    if (file != NULL)
    {
        void *prg = (void *)elf_load(file->filename, file->data);
        if (prg != NULL)
        {
            GlobalScheduler->makeProc("Test", prg);
        }
    }
    mfsCloseFile(file);

    while (true)
    {
        GlobalWM->draw();
        GlobalWM->update();
    }
}