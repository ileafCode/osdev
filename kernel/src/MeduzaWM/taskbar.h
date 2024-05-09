
#pragma once
#include <stdint.h>
#include "button.h"

class TaskBar
{
public:
    TaskBar(int height, uint8_t mainColorR, uint8_t mainColorG, uint8_t mainColorB);
    void update();
    void draw();

    int getHeight() { return this->height; };
    uint32_t getMainColorR() { return this->mainColorR; };
    uint32_t getMainColorG() { return this->mainColorG; };
    uint32_t getMainColorB() { return this->mainColorB; };
    void setMainColor(uint8_t r, uint8_t g, uint8_t b)
    { 
        this->mainColorR = r;
        this->mainColorG = g;
        this->mainColorB = b;
    }

private:
    Button mainButton;
    const int height;
    uint8_t mainColorR, mainColorG, mainColorB;
};