
#include "taskbar.h"
#include "../memory/heap.h"
#include "../cstr.h"
#include "../userinput/mouse.h"
#include "../userinput/keyboard.h"
#include "../stdio/stdio.h"

extern uint32_t RGBtoHex(uint8_t r, uint8_t g, uint8_t b);

void mainButtonTask()
{
    // Do nothing
    printf("You clicked me!");
}

TaskBar::TaskBar(int height, uint8_t mainColorR, uint8_t mainColorG, uint8_t mainColorB)
    : height(height), mainColorR(mainColorR), mainColorG(mainColorG), mainColorB(mainColorB),
    mainButton("MeduzaOS", 10, 5, 80, height - 10, 100, 100, 127)
{
    mainButton.setAction((void (*))mainButtonTask);
}

void TaskBar::update()
{
    mainButton.update();
}

void TaskBar::draw()
{
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < GlobalRenderer->TargetFramebuffer->Width; x++)
        {
            GlobalRenderer->PutPixDB(x, y,
                RGBtoHex(this->mainColorR - y, this->mainColorG - y, this->mainColorB - y));
        }
    }

    mainButton.draw();
}