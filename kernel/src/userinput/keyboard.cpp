#include "keyboard.h"
#include "../paging/PageFrameAllocator.h"
#include "../IO.h"

static char *buffer;
//static char* tempBuffer;
int bufferIdx = 0;
char last_ch = 0;
uint8_t lastScancode = 0;

bool isKeyboardEnabled = false;
bool isBufferEnabled = false;

bool isLeftShiftPressed = false;
bool isRightShiftPressed = false;

bool isCapsEnabled = false;

void PrepareKeyboard()
{
    buffer = (char *)malloc(512);
    memset(buffer, 0, 512);

    //tempBuffer = (char *)malloc(512);
    //memset(tempBuffer, 0, 512);
}

// Used differently
uint8_t lastSC = 0;

void HandleKeyboard(uint8_t scancode)
{
    if (inb(0x64) & 0x20) return;
    while (inb(0x64) & 2);
    lastScancode = scancode;

    if (!isKeyboardEnabled)
        return;

    switch (scancode)
    {
    case LeftShift:
        isLeftShiftPressed = true;
        return;
    case LeftShift + 0x80:
        isLeftShiftPressed = false;
        return;
    case RightShift:
        isRightShiftPressed = true;
        return;
    case RightShift + 0x80:
        isRightShiftPressed = false;
        return;
    case Enter:
        last_ch = '\n';
        GlobalRenderer->Next();
        buffer[bufferIdx] = 0;
        bufferIdx = 0;
        return;
    case Spacebar:
        last_ch = ' ';
        buffer[bufferIdx] = ' ';
        bufferIdx++;
        GlobalRenderer->PutChar(' ');
        return;
    case BackSpace:
        last_ch = '\b';
        if (bufferIdx == 0) return;
        if (isBufferEnabled)
        {
            bufferIdx--;
            buffer[bufferIdx] = 0;
        }
        GlobalRenderer->ClearChar();
        return;
    case Caps:
        if (isCapsEnabled == false)
            isCapsEnabled = true;
        else if (isCapsEnabled == true)
            isCapsEnabled = false;
        return;
    }

    switch (lastSC)
    {
    case LeftControl:
        if (scancode == 0x2E) // 'C'
            GlobalRenderer->Print("^C"); // Force close
        lastSC = 0;
        return;
    }

    char ascii = QWERTYKeyboard::Translate(scancode, isLeftShiftPressed | isRightShiftPressed, isCapsEnabled);

    if (scancode < 0x3A && ascii != 0)
    {
        GlobalRenderer->PutChar(ascii);
        last_ch = ascii;
        if (isBufferEnabled)
        {
            buffer[bufferIdx] = ascii;
            bufferIdx++;
        }
    }
    lastSC = scancode;
}

uint8_t KeyboardGetScancode()
{
    uint8_t sc = lastScancode;
    lastScancode = 0;
    return sc;
}

char KeyboardGetChar()
{
    isKeyboardEnabled = true;
    last_ch = 0;
    while (last_ch == 0)
        ;
    char chr = last_ch;
    last_ch = 0;
    isKeyboardEnabled = false;
    return chr;
}

char *KeyboardGetStr()
{
    isKeyboardEnabled = true;
    isBufferEnabled = true;

    while (last_ch != '\n');
    last_ch = 0;

    //for (int i = 0; i < 512; i++) {
    //    tempBuffer[i] = buffer[i];
    //}

    //memset(buffer, 0, 512);
    
    bufferIdx = 0;

    isBufferEnabled = false;
    isKeyboardEnabled = false;
    return buffer;
}