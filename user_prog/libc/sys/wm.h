
#ifndef __MEDUZAOS_WM
#define __MEDUZAOS_WM

#include <stdint.h>

#define mouseLeft 0b00000001
#define mouseMiddle 0b00000100
#define mouseRight 0b00000010

typedef struct
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
} window_t;

window_t *wm_requestWindow(char *name, int width, int height);
void wm_closeWindow(window_t *window);

#endif