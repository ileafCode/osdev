#include "kernelUtil.h"
#include "scheduling/pit/pit.h"
#include "stdio/stdio.h"
#include "fs/MeduzaFS.h"
#include "fs/fat.h"
#include "elf.h"
#include "fs/vfs.h"
#include "cstr.h"
#include "scheduling/task/sched.h"

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
            elf_program *prg = elf_load(file->filename, file->data);

            if (prg != NULL)
            {
                ((void (*)(void))prg->entryPoint)();
                free(prg);
            }
            else
                printf("[%o9ERROR%oF]: %s is not of ELF format.\n", args[0]);
            mfsCloseFile(file);
        }
        else
            printf("[%o9ERROR%oF]: No file found named %s\n", args[0]);
    }
    else if (strcmp((char *)command, "help"))
    {
        printf("=== Help Menu ===\n");
        printf("echo <args> - Prints out the arguments\n");
        printf("clear - Clears the screen\n");
        printf("ls - Lists the files and directories in the current directory\n");
        printf("cd <dirpath> - Changes the directory\n");
        printf("run <filepath> - Runs an ELF file\n");
        printf("help - Prints this menu\n");
    }
    else if (strcmp((char *)command, (char *)0));
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

extern "C" void _start(BootInfo *bootInfo)
{
    KernelInfo kernelInfo = InitializeKernel(bootInfo);

    vfsMount('A');

    mfsInit();

    uint32_t largestExtendedFunc;
    uint32_t eax, ebx, ecx, edx;

    uint32_t largestStandardFunc;
    char vendor[13];
    cpuid(0, &largestStandardFunc, (uint32_t *)(vendor + 0),
        (uint32_t *)(vendor + 8),
        (uint32_t *)(vendor + 4));
    vendor[12] = '\0';

    cpuid(0x80000000, &largestExtendedFunc, &ebx, &ecx, &edx);

    if (largestExtendedFunc >= 0x80000004)
    {
        char name[48];
        cpuid(0x80000002, (uint32_t *)(name + 0),
            (uint32_t *)(name + 4),
            (uint32_t *)(name + 8),
            (uint32_t *)(name + 12));
        cpuid(0x80000003, (uint32_t *)(name + 16),
            (uint32_t *)(name + 20),
            (uint32_t *)(name + 24),
            (uint32_t *)(name + 28));
        cpuid(0x80000004, (uint32_t *)(name + 32),
            (uint32_t *)(name + 36),
            (uint32_t *)(name + 40),
            (uint32_t *)(name + 44));

        const char *pname = name;
        while (*pname == ' ')
            ++pname;

        printf("CPU Vendor: %s\n", vendor);
        printf("CPU Name: %s\n", pname);
    }

    GlobalScheduler = new Scheduler();
    GlobalScheduler->schedule();
    GlobalScheduler->schedule();
    GlobalScheduler->schedule();
    GlobalScheduler->schedule();
    GlobalScheduler->schedule();
    GlobalScheduler->schedule();
    GlobalScheduler->schedule();

    asm("sti");

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

    while (true)
    {
        printf("%oEroot%oF@%oDpcname%oF [%oC/%oF] $ ");
        char *str = KeyboardGetStr();
        parse(str);
    }
}