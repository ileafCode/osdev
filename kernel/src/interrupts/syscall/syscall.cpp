
#include "syscall.h"
#include "../../paging/PageFrameAllocator.h"
#include "../../memory/heap.h"
#include "../../userinput/mouse.h"
#include "../../userinput/keyboard.h"
#include "../../userinput/kbScancodeTranslation.h"
#include "../../BasicRenderer.h"
#include "../../scheduling/task/sched.h"
#include "../../fs/fat/ff.h"
#include "../../scheduling/pit/pit.h"

#define FOREACH_SYSCALL(SYSCALL) \
    SYSCALL(EXIT)                \
    SYSCALL(MAKE_PROCESS)        \
    SYSCALL(DELETE_PROCESS)      \
    SYSCALL(FILE_OPEN)           \
    SYSCALL(FILE_CLOSE)          \
    SYSCALL(FILE_READ)           \
    SYSCALL(FILE_WRITE)          \
    SYSCALL(FILE_TELL)           \
    SYSCALL(FILE_SEEK)           \
    SYSCALL(CONSOLE_READ)        \
    SYSCALL(CONSOLE_WRITE)       \
    SYSCALL(MALLOC)              \
    SYSCALL(FREE)                \
    SYSCALL(SLEEPFUNC)           \
    SYSCALL(SHUTDOWN)            \
    SYSCALL(REBOOT)              \

#define GENERATE_STRING(STRING) #STRING,

static const char *syscall_names[] = {
    FOREACH_SYSCALL(GENERATE_STRING)};

struct retVal
{
    uint64_t ret1;
    uint64_t ret2;
    uint64_t ret3;
    uint64_t ret4;
} __attribute__((packed));

#define SPECIAL_RETADDRESS 0x2000

#define KBRD_INTRFC 0x64

#define KBRD_BIT_KDATA 0
#define KBRD_BIT_UDATA 1
 
#define KBRD_IO 0x60
#define KBRD_RESET 0xFE
 
#define bit(n) (1<<(n))
#define check_flag(flags, n) ((flags) & bit(n))

void reboot()
{
    uint8_t temp;
    asm volatile ("cli"); /* disable all interrupts */
 
    /* Clear all keyboard buffers (output and command buffers) */
    do
    {
        temp = inb(KBRD_INTRFC); /* empty user data */
        if (check_flag(temp, KBRD_BIT_KDATA) != 0)
            inb(KBRD_IO); /* empty keyboard data */
    } while (check_flag(temp, KBRD_BIT_UDATA) != 0);
 
    outb(KBRD_INTRFC, KBRD_RESET); /* pulse CPU reset line */
loop:
    asm volatile ("hlt"); /* if that didn't work, halt the CPU */
    goto loop; /* if a NMI is received, halt again */
}

__attribute__((interrupt)) void Syscall0x80(interrupt_frame *frame)
{
    uint64_t type;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;

    asm volatile(
        "movq %%r8, %0\n\t"
        "movq %%r9, %1\n\t"
        "movq %%r10, %2\n\t"
        "movq %%r11, %3\n\t"
        : "=r"(type), "=r"(r9), "=r"(r10), "=r"(r11) // output operands
    );

    retVal *ret = (retVal *)SPECIAL_RETADDRESS;

    //debug_printf("SYSCALL [\n    Type: %s\n    R9: %x\n    R10: %x\n    R11: %x\n]\n",
    //              syscall_names[type], r9, r10, r11);

    switch (type)
    {
    case 0:
    {
        debug_printf("Exit PID %d with Code %d\n", sched_getCurPID(), r9);
        sched_delSelf();
        break;
    }
    case 1: // Make process
    {
        break;
    }
    case 2: // Delete process
    {
        break;
    }
    case 3: // Open file
    {
        FIL *fp = (FIL *)malloc(sizeof(FIL));
        const TCHAR *path = (const TCHAR *)r9;
        BYTE mode = (BYTE)r10;
        FRESULT res = f_open(fp, path, mode);

        if (res)
        {
            debug_printf("SYSCALL: Open failed. Reason: %d\n", res);
            ret->ret1 = 0;
            break;
        }
        ret->ret1 = (uint64_t)(fp);
        break;
    }
    case 4: // Close file
    {
        FIL *fp = (FIL *)r9;
        FRESULT res = f_close(fp);

        if (res)
        {
            debug_printf("SYSCALL: Close failed. Reason: %d\n", res);
            ret->ret1 = -1;
            break;
        }
        ret->ret1 = 0;
        break;
    }
    case 5: // Read from file
    {
        uint64_t stringAddr = r10;
        int length = (int)r11;

        FIL *filePtr = (FIL *)r9;
        if (filePtr == NULL)
            break;
        UINT bytesRead = 0;
        FRESULT res = f_read(filePtr, (void *)(stringAddr), length, &bytesRead);

        if (res)
        {
            debug_printf("SYSCALL: Read failed. Reason: %d\n", res);
            ret->ret1 = 0;
            break;
        }

        ret->ret1 = bytesRead;
        break;
    }
    case 6: // Write to a file
    {
        uint64_t stringAddr = r10;
        int length = (int)r11;

        FIL *filePtr = (FIL *)r9;
        if (filePtr == NULL)
            break;
        UINT bytesWritten = 0;
        FRESULT res = f_write(filePtr, (void *)(stringAddr), length, &bytesWritten);

        if (res)
        {
            debug_printf("SYSCALL: Write failed. Reason: %d\n", res);
            ret->ret1 = 0;
            break;
        }

        ret->ret1 = bytesWritten;
        break;
    }
    case 7: // tell
    {
        FIL *filePtr = (FIL *)r9;
        if (filePtr == NULL)
            break;
        ret->ret1 = (uint64_t)f_tell(filePtr);
        break;
    }
    case 8: // seek
    {
        FIL *filePtr = (FIL *)r9;
        if (filePtr == NULL)
            break;
        FRESULT res = f_lseek(filePtr, r10);
        if (res)
        {
            debug_printf("SYSCALL: Seek failed. Reason: %d\n", res);
            ret->ret1 = 1;
            break;
        }
        ret->ret1 = 0;
        break;
    }
    case 9: // get file size
    {
        FIL *filePtr = (FIL *)r9;
        if (filePtr == NULL)
            break;
        ret->ret1 = f_size(filePtr);
    }
    case 10: // Read scancode from console
    {
        ret->ret1 = KeyboardGetScancode();

        static bool shift = false, caps = false;

        switch (ret->ret1)
        {
        case LeftShift:
            shift = true;
            break;
        case LeftShift + 0x80:
            shift = false;
            break;
        case RightShift:
            shift = true;
            break;
        case RightShift + 0x80:
            shift = false;
            break;
        case Caps:
            caps = !caps;
            break;
        }

        if (ret->ret1 > 58)
            break;

        if (caps)
            ret->ret2 = QWERTYKeyboard::ASCII_table_cl[ret->ret1];
        else if (shift)
            ret->ret2 = QWERTYKeyboard::ASCII_table_shift[ret->ret1];
        else
            ret->ret2 = QWERTYKeyboard::ASCII_table_lower[ret->ret1];
        break;
    }
    case 11: // Write to console
    {
        if (r9 < 0x20) // Command mode
        {
            switch (r9)
            {
            case 0x07: // Bell
                debug_printf("*BEEP*\n");
                break;
            case 0x08: // Backspace
                GlobalRenderer->PutChar('\b');
                break;
            case 0x0A: // Newline
                GlobalRenderer->PutChar('\n');
                break;
            case 0x1D:
            {
                GlobalRenderer->Clear(false);
                break;
            }
            case 0x1E: // Set cursor position
            {
                GlobalRenderer->cursorPos.X = (long)r10 * GlobalRenderer->PSF1_Font->psf1_Header->charsize;
                GlobalRenderer->cursorPos.Y = (long)r11 * 16;
                break;
            }
            case 0x1F: // Change color
            {
                GlobalRenderer->color = r10 & 0xFFFFFFFF;
                GlobalRenderer->bgColor = r11 & 0xFFFFFFFF;
                break;
            }
            }
            break;
        }

        GlobalRenderer->PutChar((uint8_t)r9);
        break;
    }
    case 12: // malloc
    {
        ret->ret1 = (uint64_t)malloc(r9);
        break;
    }
    case 13: // free
    {
        free((void *)r9);
        break;
    }
    case 14: // sleep
    {
        ret->ret1 = (uint64_t)PIT::Sleep;
        break;
    }
    case 15: // shutdown
    {
        debug_printf("BEEP BOOP (shutdown)\n");
        break;
    }
    case 16: // reboot
    {
        reboot();
        break;
    }
    case 17: // opendir
    {
        ret->ret1 = (uint64_t)f_opendir((DIR *)r9, (const TCHAR *)r10);
        break;
    }
    case 18: // closedir
    {
        ret->ret1 = (uint64_t)f_closedir((DIR *)r9);
        break;
    }
    case 19: // readdir
    {
        ret->ret1 = (uint64_t)f_readdir((DIR *)r9, (FILINFO *)r10);
        break;
    }
    }
}