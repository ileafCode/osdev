#pragma once
#include "math.h"
#include "cstr.h"
#include "Framebuffer.h"
#include "simpleFonts.h"
#include "memory/heap.h"
#include <stdint.h>

class BasicRenderer
{
public:
    BasicRenderer(Framebuffer *targetFramebuffer, PSF1_FONT *psf1_Font);
    uint32_t *buffer;
    Point cursorPos;
    Framebuffer *TargetFramebuffer;
    PSF1_FONT *PSF1_Font;
    PSF2_FONT *PSF2_Font;
    uint32_t MouseCursorBuffer[16 * 16];
    uint32_t MouseCursorBufferAfter[16 * 16];
    unsigned int color = 0x00FFFFFF;
    unsigned int bgColor = 0x00000000;
    unsigned int clearColor = 0x00000000;
    void Print(const char *str);
    void PutChar(uint8_t chr, unsigned int xOff, unsigned int yOff);
    void PutCharDB(uint8_t chr, unsigned int xOff, unsigned int yOff);
    void PutChar(uint8_t chr);
    void PutPix(uint32_t x, uint32_t y, uint32_t colour);
    uint32_t GetPix(uint32_t x, uint32_t y);
    void PutPixDB(uint32_t x, uint32_t y, uint32_t colour);
    uint32_t GetPixDB(uint32_t x, uint32_t y);
    void FlipDB();
    void ClearDB();
    void ClearChar();
    void Clear();
    void Next();
    void DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, uint32_t colour);
    void ClearMouseCursor(uint8_t *mouseCursor, Point position);
    void EnableSecondBuffer();
    bool MouseDrawn;
};

extern BasicRenderer *GlobalRenderer;