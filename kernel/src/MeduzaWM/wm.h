
#pragma once
#include <stdint.h>
#include "../BasicRenderer.h"
#include "../vector/vector.h"
#include "taskbar.h"
#include "image.h"
#define MAX_WINDOWS 200

struct window_t
{
    // Window info
    char name[32];
    int16_t x, y, width, height;
    uint32_t *pixels;
    int focus;
    int id;

    // Input info
    int mouseX;
    int mouseY;
    uint64_t mouseButtons;
    uint8_t sc;
    int shouldClose;
} __attribute__((packed));

class WindowManager
{
public:
    WindowManager();
    void update();
    void draw();
    window_t *makeWindow(char *name, uint16_t width, uint16_t height);
    void deleteWindow(window_t *window);
    void drawWindow(window_t *window);

private:
    bool redraw = true;
    Image mouseCur;
    TaskBar taskBar;
    int windowSize = 0;
    window_t *focusedWin;
    window_t *windows[MAX_WINDOWS];
};

extern WindowManager *GlobalWM;