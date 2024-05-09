
#pragma once
#include <stdint.h>

class Button
{
public:
    Button(char *title, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t r, uint8_t g, uint8_t b);
    void setAction(void *action);
    void update();
    void draw();
private:
    bool heldDown = false;
    void *action;
    char *title;
    int16_t x, y, width, height;
    uint8_t r, g, b;
};