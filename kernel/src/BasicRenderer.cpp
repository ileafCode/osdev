#include "BasicRenderer.h"

BasicRenderer *GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer *targetFramebuffer, PSF1_FONT *psf1_Font)
{
    TargetFramebuffer = targetFramebuffer;
    PSF1_Font = psf1_Font;
    color = 0xffffffff;
    cursorPos = {0, 0};
}

void BasicRenderer::EnableSecondBuffer()
{
    this->buffer = (uint32_t *)malloc(this->TargetFramebuffer->PixelsPerScanLine * this->TargetFramebuffer->Height * 4);
    if (this->buffer == NULL)
    {
        Print("Not enough memory for double buffering.");
        while (1)
            asm("cli; hlt");
    }
}

void BasicRenderer::PutPix(uint32_t x, uint32_t y, uint32_t colour)
{
    *(uint32_t *)((uint64_t)TargetFramebuffer->BaseAddress + (x * 4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = colour;
}

uint32_t BasicRenderer::GetPix(uint32_t x, uint32_t y)
{
    return *(uint32_t *)((uint64_t)TargetFramebuffer->BaseAddress + (x * 4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::PutPixDB(uint32_t x, uint32_t y, uint32_t colour)
{
    *(buffer + (y * TargetFramebuffer->Width + x)) = colour;
}

uint32_t BasicRenderer::GetPixDB(uint32_t x, uint32_t y)
{
    return *(buffer + (y * TargetFramebuffer->Width + x));
}

void BasicRenderer::FlipDB()
{
    memcpy((void *)TargetFramebuffer->BaseAddress, (void *)buffer, TargetFramebuffer->Width * TargetFramebuffer->Height * 4);
}

void BasicRenderer::ClearDB()
{
    memset32((void *)buffer, 0, TargetFramebuffer->Width * TargetFramebuffer->Height);
}

void BasicRenderer::ClearMouseCursor(uint8_t *mouseCursor, Point position)
{
    if (!MouseDrawn)
        return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16)
        xMax = differenceX;
    if (differenceY < 16)
        yMax = differenceY;

    for (int y = 0; y < yMax; y++)
    {
        for (int x = 0; x < xMax; x++)
        {
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
                if (GetPixDB(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y * 16])
                    PutPixDB(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
        }
    }
}

void BasicRenderer::DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, uint32_t colour)
{
    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16)
        xMax = differenceX;
    if (differenceY < 16)
        yMax = differenceY;

    for (int y = 0; y < yMax; y++)
    {
        for (int x = 0; x < xMax; x++)
        {
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                MouseCursorBuffer[x + y * 16] = GetPixDB(position.X + x, position.Y + y);
                PutPixDB(position.X + x, position.Y + y, colour);
                MouseCursorBufferAfter[x + y * 16] = GetPixDB(position.X + x, position.Y + y);
            }
        }
    }

    MouseDrawn = true;
}

void BasicRenderer::Clear()
{
    cursorPos.X = 0;
    cursorPos.Y = 0;

    uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
    uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = TargetFramebuffer->Height;
    uint64_t fbSize = TargetFramebuffer->BufferSize;

    for (int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline++)
    {
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
        for (uint32_t *pixPtr = (uint32_t *)pixPtrBase; pixPtr < (uint32_t *)(pixPtrBase + bytesPerScanline); pixPtr++)
        {
            *pixPtr = clearColor;
        }
    }
}

void BasicRenderer::ClearChar()
{
    if (cursorPos.X == 0)
    {
        cursorPos.X = TargetFramebuffer->Width;
        cursorPos.Y -= 16;
        if (cursorPos.Y < 0)
            cursorPos.Y = 0;
    }

    unsigned int xOff = cursorPos.X;
    unsigned int yOff = cursorPos.Y;

    unsigned int *pixPtr = (unsigned int *)TargetFramebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff - 8; x < xOff; x++)
        {
            *(unsigned int *)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = clearColor;
        }
    }

    cursorPos.X -= 8;

    if (cursorPos.X < 0)
    {
        cursorPos.X = TargetFramebuffer->Width;
        cursorPos.Y -= 16;
        if (cursorPos.Y < 0)
            cursorPos.Y = 0;
    }
}

void BasicRenderer::Next()
{
    cursorPos.X = 0;
    cursorPos.Y += 16;
}

void BasicRenderer::Print(const char *str)
{
    char *chr = (char *)str;
    while (*chr != 0)
    {
        PutChar(*chr, cursorPos.X, cursorPos.Y);
        cursorPos.X += 8;
        if (cursorPos.X + 8 > TargetFramebuffer->Width)
        {
            cursorPos.X = 0;
            cursorPos.Y += 16;
        }
        chr++;
    }
}

void BasicRenderer::PutChar(uint8_t chr, unsigned int xOff, unsigned int yOff)
{
    unsigned int *pixPtr = (unsigned int *)TargetFramebuffer->BaseAddress;
    char *fontPtr = (char *)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
                *(unsigned int *)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = color;
            else
                *(unsigned int *)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = bgColor;
        }
        fontPtr++;
    }
}

void BasicRenderer::PutCharDB(uint8_t chr, unsigned int xOff, unsigned int yOff)
{
    unsigned int *pixPtr = (unsigned int *)buffer;
    char *fontPtr = (char *)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
                *(unsigned int *)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = color;
            //else
            //    *(unsigned int *)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = bgColor;
        }
        fontPtr++;
    }
}

void BasicRenderer::PutChar(uint8_t chr)
{
    if (chr == 0)
        return;
    if (chr == '\n')
    {
        this->Next();
        return;
    }
    else if (chr == '\b')
    {
        this->ClearChar();
        return;
    }

    if (cursorPos.X + 8 > TargetFramebuffer->Width)
    {
        cursorPos.X = 0;
        cursorPos.Y += 16;
    }

    if (cursorPos.Y + 16 > TargetFramebuffer->Height)
    {
        this->Clear();
        cursorPos.X = 0;
        cursorPos.Y = 0;
    }

    PutChar(chr, cursorPos.X, cursorPos.Y);
    cursorPos.X += 8;
}