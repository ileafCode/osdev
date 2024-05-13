
#include "wm.h"
#include "../memory/heap.h"
#include "../cstr.h"
#include "../userinput/mouse.h"
#include "../userinput/keyboard.h"
#include "../stdio/stdio.h"
#include "../paging/PageFrameAllocator.h"

// absolutely terrible window manager :0

WindowManager *GlobalWM;

uint32_t RGBtoHex(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

#define WIN_OFFSET 35

WindowManager::WindowManager() : taskBar(40, 100, 100, 127), mouseCur("mous.tga")
{
    // GlobalRenderer->ClearDB();
    //  this->windows.reserve(5);
}

int mouse_oldX = 0;
int mouse_oldY = 0;

void WindowManager::update()
{
    bool canClick = true;
    for (int i = 0; i < windowSize; i++)
    {
        window_t *window = this->windows[i];
        if (window == NULL)
            continue;

        if (MousePosition.X != mouse_oldX || MousePosition.Y != mouse_oldY)
            this->redraw = true;

        if (window->focus)
            window->sc = KeyboardGetScancode();

        if (MousePosition.X >= window->x &&
            MousePosition.X <= window->x + window->width &&
            MousePosition.Y >= window->y &&
            MousePosition.Y <= window->y + WIN_OFFSET)
        {
            if ((mouseButtonData & PS2Leftbutton))
            {
                // Mouse left button is pressed
                canClick = false;
                this->redraw = true;

                focusedWin->focus = false;

                window->focus = true;
                focusedWin = window;
                window_t *temp = this->windows[0];
                this->windows[0] = window;
                this->windows[i] = temp;

                window->x += (int16_t)MousePosition.X - mouse_oldX;
                window->y += (int16_t)MousePosition.Y - mouse_oldY;

                // if (window->x < 0)
                //     window->x = 0;
                // else if (window->y < 0)
                //     window->y = 0;
                // else if (window->x > GlobalRenderer->TargetFramebuffer->Width - window->width)
                //     window->x = GlobalRenderer->TargetFramebuffer->Width  - window->width;
                // else if (window->y > GlobalRenderer->TargetFramebuffer->Height - window->height)
                //     window->y = GlobalRenderer->TargetFramebuffer->Height - window->height;
                break;
            }
        }
        else if (MousePosition.X >= window->x &&
                 MousePosition.X <= window->x + window->width &&
                 MousePosition.Y >= window->y + WIN_OFFSET &&
                 MousePosition.Y <= window->y + window->height + WIN_OFFSET)
        {
            if ((mouseButtonData & PS2Leftbutton))
            {
                canClick = false;
                this->redraw = true;
                focusedWin->focus = false;

                window->focus = true;
                focusedWin = window;
                window_t *temp = this->windows[0];
                this->windows[0] = window;
                this->windows[i] = temp;
                break;
            }
            // Send mouse position data relative to the window
            if (window->focus)
            {
                window->mouseButtons = mouseButtonData;
                window->mouseX = MousePosition.X - window->x;
                window->mouseY = MousePosition.Y - (window->y + WIN_OFFSET);
            }
        }
        else
        {
            if ((mouseButtonData & PS2Leftbutton))
            {
                this->redraw = true;
                focusedWin->focus = false;
                focusedWin = NULL;
            }
        }
    }

    mouse_oldX = MousePosition.X;
    mouse_oldY = MousePosition.Y;
}

void WindowManager::draw()
{
    // if (!redraw) return;
    GlobalRenderer->ClearDB();

    //this->img.draw(0, 0, GlobalRenderer->TargetFramebuffer->Width, GlobalRenderer->TargetFramebuffer->Height);

    //Draw taskbar
    //this->taskBar.draw();

    // Draw windows
    for (int i = this->windowSize - 1; i >= 0; i--)
    {
        if (this->windows[i] == NULL)
            continue;
        if (this->windows[i]->pixels == NULL)
            continue;
        drawWindow(this->windows[i]);
    }

    // Draw cursor
    GlobalRenderer->DrawOverlayMouseCursor(MousePointer, MousePosition, 0xffffffff);
    //this->mouseCur.draw(MousePosition.X, MousePosition.Y);

    // Flip buffers
    GlobalRenderer->FlipDB();
    redraw = false;
}

int prevX = 0;
int prevY = 0;

window_t *WindowManager::makeWindow(char *name, uint16_t width, uint16_t height)
{
    if (width < 320)
        width = 320;
    if (height < 240)
        height = 240;
    window_t *window = (window_t *)malloc(sizeof(window_t));
    window->x = 10 + prevX;
    window->y = 10 + prevY;
    prevX += WIN_OFFSET;
    prevY += WIN_OFFSET + 5;
    window->width = width;
    window->height = height;
    window->focus = true;

    for (int i = 0; i < windowSize; i++)
        this->windows[i]->focus = false;

    window->id = windowSize;
    window->pixels = (uint32_t *)malloc(width * height * 4);

    strcpy(window->name, name);
    memset((void *)window->pixels, 0x7F, width * height * 4);

    this->windows[windowSize++] = window;

    this->focusedWin = window;
    this->redraw = true;
    return window;
}

void WindowManager::deleteWindow(window_t *window)
{
    if (window == NULL)
        return;

    if (window->pixels != NULL)
    {
        free(window->pixels);
        window->pixels = NULL;
    }

    for (int i = this->windowSize - 1; i >= 0; i--)
    {
        if (this->windows[i] == window)
        {
            if (this->windowSize > 1)
            {
                for (int j = i; j < this->windowSize - 1; j++)
                    this->windows[j] = this->windows[j + 1];
            }
            this->windowSize--;
            break;
        }
    }

    free(window);
    window = NULL;
}

void WindowManager::drawWindow(window_t *window)
{
    if (window == NULL)
        return;
    int windowX = window->x - 1;
    int windowY = window->y;
    int windowWidth = window->width;
    int windowHeight = window->height;
    int titleLength = strlen(window->name);
    uint32_t titleColor = window->focus ? RGBtoHex(100 - WIN_OFFSET, 100 - WIN_OFFSET, 127 - WIN_OFFSET) : RGBtoHex(115 - WIN_OFFSET, 115 - WIN_OFFSET, 127 - WIN_OFFSET);

    // Draw the titlebar
    for (int y = 0; y < WIN_OFFSET; y++)
    {
        for (int x = 0; x < windowWidth; x += 8)
        {
            GlobalRenderer->PutPixDB(windowX + x, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 1, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 2, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 3, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 4, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 5, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 6, windowY + y, titleColor);
            GlobalRenderer->PutPixDB(windowX + x + 7, windowY + y, titleColor);
        }
    }

    // Draw the borders
    // for (int y = WIN_OFFSET; y < windowHeight + 1 + WIN_OFFSET; y++)
    //{
    //    GlobalRenderer->PutPixDB(windowX, windowY + y, titleColor);
    //    GlobalRenderer->PutPixDB(windowX + windowWidth, windowY + y, titleColor);
    //}
    // for (int x = 0; x < windowWidth; x++)
    //    GlobalRenderer->PutPixDB(windowX + x, windowY + windowHeight + WIN_OFFSET, titleColor);

    // Draw the title
    for (int i = 0; i < titleLength; i++)
    {
        GlobalRenderer->PutCharDB(window->name[i], windowX + 10 + (i * 8), windowY + 10);
    }

    GlobalRenderer->color = 0xFFFFFFFF;
    // Draw the buffer of the window
    for (int y = 0; y < windowHeight; y++)
        for (int x = 0; x < windowWidth; x += 4)
        {
            GlobalRenderer->PutPixDB(windowX + x, windowY + y + WIN_OFFSET, *(window->pixels + (y * windowWidth + x)));
            GlobalRenderer->PutPixDB(windowX + x + 1, windowY + y + WIN_OFFSET, *(window->pixels + (y * windowWidth + x + 1)));
            GlobalRenderer->PutPixDB(windowX + x + 2, windowY + y + WIN_OFFSET, *(window->pixels + (y * windowWidth + x + 2)));
            GlobalRenderer->PutPixDB(windowX + x + 3, windowY + y + WIN_OFFSET, *(window->pixels + (y * windowWidth + x + 3)));
        }
}