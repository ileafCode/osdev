
#include "button.h"
#include "../cstr.h"
#include "../BasicRenderer.h"
#include "../userinput/mouse.h"

extern uint32_t RGBtoHex(uint8_t r, uint8_t g, uint8_t b);

Button::Button(char *title, int16_t x, int16_t y, int16_t width, int16_t height,
               uint8_t r, uint8_t g, uint8_t b)
    : title(title), x(x), y(y), width(width), height(height), r(r), g(g), b(b)
{
}

void Button::setAction(void *action)
{
    this->action = action;
}

void Button::update()
{
    if (MousePosition.X >= this->x &&
        MousePosition.X <= this->x + this->width &&
        MousePosition.Y >= this->y &&
        MousePosition.Y <= this->y + this->height &&
        mouseButtonData & PS2Leftbutton &&
        heldDown == false)
    {
        heldDown = true;
        ((void (*)(void))action)();
    }

    if (!(mouseButtonData & PS2Leftbutton))
        heldDown = false;
}

void Button::draw()
{
    for (int y = this->y; y < this->height + this->y; y++)
        for (int x = this->x; x < this->width + this->x; x++)
        {
            uint32_t color = RGBtoHex(this->r, this->g, this->b);
            if (x == this->x || y == this->y ||
                y == this->height + this->y - 1 || x == this->width + this->x - 1)
                color = RGBtoHex(this->r / 2, this->g / 2, this->b / 2);

            GlobalRenderer->PutPixDB(x, y, color);
        }

    for (int i = 0; i < strlen(this->title); i++)
    {
        GlobalRenderer->PutCharDB(this->title[i],
                                  this->x + (i * 8) + ((this->width / 2) - ((strlen(this->title) * 8) / 2)),
                                  this->y + ((this->height / 2) - 8));
    }
}