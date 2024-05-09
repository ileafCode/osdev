
/*
--- MeduzaOS Window Program ---
--- Basic windowed program for MeduzaOS ---
*/

#include <sys/wm.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    window_t *window = wm_requestWindow("Window", 800, 600);
    int x = 0;
    while (1)
    {
        for (int i = 0; i < window->width * window->height; i++)
            *(uint32_t *)(window->pixels + i) = i + x;
        if (window->sc == 0x01)
            break;
        x++;
    }
    wm_closeWindow(window);
    return 0;
}
