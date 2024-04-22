#include "panic.h"
#include "BasicRenderer.h"
#include "stdio/stdio.h"

void Panic(const char* panicMessage){
    GlobalRenderer->clearColor = 0x00000000;
    GlobalRenderer->Clear();

    GlobalRenderer->cursorPos = {0, 0};

    printf("%o9[ERROR]%oF: KERNEL_PANIC\n%o9[REASON]%oF: %s", panicMessage);
}