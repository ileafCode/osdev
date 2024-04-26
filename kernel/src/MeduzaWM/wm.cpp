
#include "wm.h"
#include "../memory/heap.h"
#include "../cstr.h"
#include "../userinput/mouse.h"

// absolutely terrible window manager :0

WindowManager *GlobalWM;

#define WIN_OFFSET 35

WindowManager::WindowManager()
{
    // this->windows.reserve(5);
}

int mouse_oldX = 0;
int mouse_oldY = 0;

void WindowManager::update()
{
    for (int i = 0; i < windowSize; i++)
    {
        window_t *window = this->windows[i];
        if (window == NULL)
            continue;
        
        // Send mouse position data relative to the window
        if (window->focus)
        {
            window->mouseX = MousePosition.X - window->x;
            window->mouseY = MousePosition.Y - (window->y + WIN_OFFSET);
        }

        if (MousePosition.X >= window->x &&
            MousePosition.X <= window->x + window->width &&
            MousePosition.Y >= window->y &&
            MousePosition.Y <= window->y + WIN_OFFSET)
        {
            // Mouse is inside square
            if ((mouseButtonData & PS2Leftbutton))
            {
                // Mouse left button is pressed
                for (int j = 0; j < this->windowSize; j++)
                {
                    window_t *window_ = this->windows[j];
                    if (window_ == NULL)
                        continue;
                    window_->focus = false;
                }

                window->focus = true;
                window_t *temp = this->windows[0];
                this->windows[0] = window;
                this->windows[i] = temp;

                window->x += MousePosition.X - mouse_oldX;
                window->y += MousePosition.Y - mouse_oldY;

                if (window->x < 0)
                    window->x = 0;
                if (window->y < 0)
                    window->y = 0;

                if (window->x + window->width > GlobalRenderer->TargetFramebuffer->Width)
                    window->x = GlobalRenderer->TargetFramebuffer->Width - window->width;

                if (window->y + window->height > GlobalRenderer->TargetFramebuffer->Height)
                    window->y = GlobalRenderer->TargetFramebuffer->Height - window->height;
                break;
            }
        }
        else if (MousePosition.X >= window->x &&
                 MousePosition.X <= window->x + window->width &&
                 MousePosition.Y >= window->y + WIN_OFFSET &&
                 MousePosition.Y <= window->y + window->height)
        {
            if ((mouseButtonData & PS2Leftbutton))
            {
                for (int j = 0; j < this->windowSize; j++)
                {
                    window_t *window_ = this->windows[j];
                    if (window_ == NULL)
                        continue;
                    window_->focus = false;
                }

                window->focus = true;
                window_t *temp = this->windows[0];
                this->windows[0] = window;
                this->windows[i] = temp;

                break;
            }
        }
    }

    mouse_oldX = MousePosition.X;
    mouse_oldY = MousePosition.Y;
}

void WindowManager::draw()
{
    for (int i = this->windowSize - 1; i >= 0; i--)
    {
        if (&this->windows[i] == NULL)
            continue;
        drawWindow(this->windows[i]);
    }
}

int prevX = 0;
int prevY = 0;

window_t *WindowManager::makeWindow(char *name, uint16_t width, uint16_t height)
{
    window_t *window = (window_t *)malloc(sizeof(window_t));
    window->x = 10 + prevX;
    window->y = 10 + prevY;
    prevX += WIN_OFFSET;
    prevY += WIN_OFFSET;
    window->width = width;
    window->height = height;
    window->focus = true;

    for (int i = 0; i < windowSize; i++)
    {
        this->windows[i]->focus = false;
        this->windows[i]->id++;
    }

    window->id = 0;
    window->pixels = (uint32_t *)malloc(width * height * 4);

    strcpy(window->name, name);
    memset((void *)window->pixels, 0x7F, width * height * 4);

    this->windows[windowSize++] = window;

    return window;
}

uint32_t RGBtoHex(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void *WindowManager::drawWindow(window_t *window)
{
    // Draw the titlebar
    for (int y = 0; y < WIN_OFFSET; y++)
        for (int x = 0; x < window->width + 8; x++)
            if (window->focus)
                GlobalRenderer->PutPixDB(window->x + x - 4, window->y + y, RGBtoHex(100 - (y), 100 - (y), 127 - (y)));
            else
            {
                GlobalRenderer->color = RGBtoHex(200, 200, 240);
                GlobalRenderer->PutPixDB(window->x + x - 4, window->y + y, RGBtoHex(115 - (y), 115 - (y), 127 - (y)));
            }

    // Draw the borders
    for (int y = WIN_OFFSET; y < window->height + 4 + WIN_OFFSET; y++)
    {
        uint32_t color = RGBtoHex(100 - WIN_OFFSET, 100 - WIN_OFFSET, 127 - WIN_OFFSET);
        if (!window->focus)
            color = RGBtoHex(115 - WIN_OFFSET, 115 - WIN_OFFSET, 127 - WIN_OFFSET);

        GlobalRenderer->PutPixDB(window->x - 4, window->y + y, color);
        GlobalRenderer->PutPixDB(window->x - 3, window->y + y, color);
        GlobalRenderer->PutPixDB(window->x - 2, window->y + y, color);
        GlobalRenderer->PutPixDB(window->x - 1, window->y + y, color);

        GlobalRenderer->PutPixDB(window->x + window->width + 0, window->y + y, color);
        GlobalRenderer->PutPixDB(window->x + window->width + 1, window->y + y, color);
        GlobalRenderer->PutPixDB(window->x + window->width + 2, window->y + y, color);
        GlobalRenderer->PutPixDB(window->x + window->width + 3, window->y + y, color);
    }

    for (int x = 0; x < window->width; x++)
    {
        uint32_t color = RGBtoHex(100 - WIN_OFFSET, 100 - WIN_OFFSET, 127 - WIN_OFFSET);
        if (!window->focus)
            color = RGBtoHex(115 - WIN_OFFSET, 115 - WIN_OFFSET, 127 - WIN_OFFSET);

        GlobalRenderer->PutPixDB(window->x + x, window->y + window->height + WIN_OFFSET + 0, color);
        GlobalRenderer->PutPixDB(window->x + x, window->y + window->height + WIN_OFFSET + 1, color);
        GlobalRenderer->PutPixDB(window->x + x, window->y + window->height + WIN_OFFSET + 2, color);
        GlobalRenderer->PutPixDB(window->x + x, window->y + window->height + WIN_OFFSET + 3, color);
    }

    // Draw the title
    for (int i = 0; i < strlen(window->name); i++)
        GlobalRenderer->PutCharDB(window->name[i], window->x + 10 + (i * 8), window->y + 10);

    GlobalRenderer->color = 0xFFFFFFFF;

    // Draw the buffer of the window
    for (int y = 0; y < window->height; y++)
        for (int x = 0; x < window->width; x++)
            GlobalRenderer->PutPixDB(window->x + x, window->y + y + WIN_OFFSET, *(window->pixels + (y * window->width + x)));
}