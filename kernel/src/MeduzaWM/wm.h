
#pragma once
#include <stdint.h>
#include "../BasicRenderer.h"
#include "../vector/vector.h"
#define MAX_WINDOWS 200

struct window_t
{
    // Window info
    char name[32];
    int16_t x, y, width, height;
    uint32_t *pixels;
    bool focus;
    int id;

    // Input info
    int mouseX;
    int mouseY;
    uint8_t sc;
};

class WindowManager
{
public:
    WindowManager();
    void update();
    void draw();
    window_t *makeWindow(char *name, uint16_t width, uint16_t height);
    void *drawWindow(window_t *window);

private:
    int windowSize = 0;
    window_t *windows[MAX_WINDOWS];
};

extern WindowManager *GlobalWM;