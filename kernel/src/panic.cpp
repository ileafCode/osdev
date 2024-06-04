#include "panic.h"
#include "BasicRenderer.h"
#include "stdio/stdio.h"

void Panic(const char *panicMessage)
{
    GlobalRenderer->clearColor = 0x00000000;
    GlobalRenderer->Clear();

    GlobalRenderer->cursorPos = {0, 0};

    printf("%v9[ERROR]%vF: KERNEL_PANIC\n%v9[REASON]%vF: %s", panicMessage);
}