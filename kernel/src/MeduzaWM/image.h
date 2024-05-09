
#pragma once
#include <stdint.h>
#include <stddef.h>

class Image
{
public:
    Image(char *path);
    void draw(int x, int y);
    void draw(int x, int y, int width, int height);
private:
    int width;
    int height;
    uint32_t *imgPtr = NULL;
};