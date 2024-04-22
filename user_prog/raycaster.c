
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mouse.h>
#include <sys/fb.h>

uint32_t *texture_data;

#define sign(x) ((x < 0) ? -1 : ((x > 0) ? 1 : 0))

#define PI2 M_PI / 2
#define PI3 3 * M_PI / 2

#define DR 0.0174533

void square(FB_DATA *fb, uint32_t color, float x, float y, int size_x, int size_y)
{
    for (int yy = y; yy < y + size_y; yy++)
    {
        for (int xx = x; xx < x + size_x; xx++)
        {
            fb_putpix(fb, color, xx, yy);
        }
    }
}

void line(FB_DATA *fb, uint32_t color, int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    while (fb_putpix(fb, color, x0, y0), x0 != x1 || y0 != y1)
    {
        int e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

#define NUM_RAYS 240
#define RAY_WIDTH 5

#define MAP_WIDTH 12
#define MAP_HEIGHT 12
#define MAP_BLOCK_SIZE 64

int map[MAP_HEIGHT][MAP_WIDTH] = {
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 0, 1, 0, 2, 0, 0, 3, 0, 4, 0, 2},
    {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
    {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
};

float px = 3.0f * MAP_BLOCK_SIZE, py = 3.0f * MAP_BLOCK_SIZE, pdx, pdy, pa;

void update()
{
    uint8_t sc = getsc();
    if (sc == 0)
        return;

    int xo = 0;
    if (pdx < 0)
        xo = -20;
    else
        xo = 20;

    int yo = 0;
    if (pdy < 0)
        yo = -20;
    else
        yo = 20;

    int ipx = px / MAP_BLOCK_SIZE, ipx_add_xo = (px + xo) / MAP_BLOCK_SIZE, ipx_sub_xo = (px - xo) / MAP_BLOCK_SIZE;
    int ipy = py / MAP_BLOCK_SIZE, ipy_add_yo = (py + yo) / MAP_BLOCK_SIZE, ipy_sub_yo = (py - yo) / MAP_BLOCK_SIZE;

    if (sc == 0x1C)
    {
        if (map[ipy_add_yo][ipx_add_xo] == 1)
            map[ipy_add_yo][ipx_add_xo] = 0;
    }

    if (sc == 0x11)
    {
        if (map[ipy][ipx_add_xo] == 0)
            px += pdx;
        if (map[ipy_add_yo][ipx] == 0)
            py += pdy;
    }
    if (sc == 0x1F)
    {
        if (map[ipy][ipx_sub_xo] == 0)
            px -= pdx;
        if (map[ipy_sub_yo][ipx] == 0)
            py -= pdy;
    }
    if (sc == 0x1E)
    {
        pa -= 0.1f;
        if (pa < 0)
            pa += 2 * M_PI;
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    }
    if (sc == 0x20)
    {
        pa += 0.1f;
        if (pa > 2 * M_PI)
            pa -= 2 * M_PI;
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    }
}

float dist(float ax, float ay, float bx, float by, float ang)
{
    return (float)sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void drawRays(FB_DATA *fb)
{
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo, disT;

    ra = pa - (DR * 30);
    if (ra < 0)
        ra += 2 * M_PI;
    if (ra > 2 * M_PI)
        ra -= 2 * M_PI;

    int MAX_DOF = max(MAP_WIDTH, MAP_HEIGHT);

    for (int r = 0; r < NUM_RAYS; r++)
    {
        // Check Horizontal
        dof = 0;
        int mIdxXH = 0, mIdxYH = 0;
        float distH = 1000000, hx = px, hy = py;
        float aTan = 1 / tan(-ra);
        if (ra > M_PI) // looking down
        {
            ry = (((int)py >> 6) << 6) - 0.0001f;
            rx = (py - ry) * aTan + px;
            yo = -MAP_BLOCK_SIZE;
            xo = -yo * aTan;
        }
        if (ra < M_PI) // looking up
        {
            ry = (((int)py >> 6) << 6) + MAP_BLOCK_SIZE;
            rx = (py - ry) * aTan + px;
            yo = MAP_BLOCK_SIZE;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == M_PI)
        {
            rx = px;
            ry = py;
            dof = MAX_DOF;
        }

        while (dof < MAX_DOF)
        {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * MAP_WIDTH + mx;

            if (mp >= 0 && mp < MAP_WIDTH * MAP_HEIGHT && map[my][mx])
            {
                hx = rx;
                hy = ry;
                distH = dist(px, py, hx, hy, ra);
                dof = MAX_DOF;
                mIdxXH = mx;
                mIdxYH = my;
            }
            else
            {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        // Check Vertical
        dof = 0;
        int mIdxXV = 0, mIdxYV = 0;
        float distV = 1000000, vx = px, vy = py;
        float nTan = -tan(ra);
        if (ra > PI2 && ra < PI3) // looking left
        {
            rx = (((int)px >> 6) << 6) - 0.0001f;
            ry = (px - rx) * nTan + py;
            xo = -MAP_BLOCK_SIZE;
            yo = -xo * nTan;
        }
        if (ra < PI2 || ra > PI3) // looking right
        {
            rx = (((int)px >> 6) << 6) + MAP_BLOCK_SIZE;
            ry = (px - rx) * nTan + py;
            xo = MAP_BLOCK_SIZE;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == M_PI)
        {
            rx = px;
            ry = py;
            dof = MAX_DOF;
        }

        while (dof < MAX_DOF)
        {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * MAP_WIDTH + mx;

            if (mp >= 0 && mp < MAP_WIDTH * MAP_HEIGHT && map[my][mx])
            {
                vx = rx;
                vy = ry;
                distV = dist(px, py, vx, vy, ra);
                dof = MAX_DOF;
                mIdxXV = mx;
                mIdxYV = my;
            }
            else
            {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }

        int mIdxX, mIdxY, side = 0;

        if (distH < distV)
        {
            rx = hx;
            ry = hy;
            disT = distH;
            mIdxX = mIdxXH;
            mIdxY = mIdxYH;
        }

        if (distV < distH)
        {
            rx = vx;
            ry = vy;
            disT = distV;
            mIdxX = mIdxXV;
            mIdxY = mIdxYV;
            side = 1;
        }

        uint32_t color;
        switch (map[mIdxY][mIdxX])
        {
        case 1:
            color = 0xFFFFFF;
            break;
        case 2:
            color = 0xFF0000;
            break;
        case 3:
            color = 0x00FF00;
            break;
        case 4:
            color = 0x0000FF;
            break;
        }

        color -= ((color & 0x202020) * side);

        // line(fb, color, px, py, rx, ry);

        // 3D
        float ca = pa - ra;
        if (ca < 0)
            ca += 2 * M_PI;
        if (ca > 2 * M_PI)
            ca -= 2 * M_PI;

        disT *= cos(ca);

        float lineH = (MAP_BLOCK_SIZE * fb->height) / disT;
        float ty_step = 32.0 / (float)lineH;
        float ty_off = 0;
        if (lineH > fb->height)
        {
            ty_off = (lineH - fb->height) / 2.f;
            lineH = fb->height;
        }

        int lineOffsetX = (r * RAY_WIDTH); // + (MAP_WIDTH * MAP_BLOCK_SIZE);
        int lineOffsetY = (fb->height - lineH) / 2;
        // square(fb, color, lineOffsetX, lineOffsetY, RAY_WIDTH, lineH);

        float ty = ty_off * ty_step;
        float tx;
        if (side != 1)
        {
            tx = (int)(rx / 2.f) % 32;
            if (ra > M_PI)
                tx = 31 - tx;
        }
        else
        {
            tx = (int)(ry / 2.f) % 32;
            if (ra > PI2 / 2 && ra < PI3)
                tx = 31 - tx;
        }

        ty += 32 * (map[mIdxY][mIdxX] - 1);

        for (int y = 0; y < lineH; y++)
        {
            uint32_t c = texture_data[(int)(ty) * 32 + (int)(tx)];
            c -= ((c & 0x202020) * side);
            square(fb, c, lineOffsetX, lineOffsetY + y, RAY_WIDTH, 1);
            ty += ty_step;
        }

        ra += DR / 4;
        if (ra < 0)
            ra += 2 * M_PI;
        if (ra > 2 * M_PI)
            ra -= 2 * M_PI;
    }
}

void draw(FB_DATA *fb)
{
    // Map
    for (int y = 0; y < MAP_HEIGHT; y++)
        for (int x = 0; x < MAP_WIDTH; x++)
            if (map[y][x])
                square(fb, 0xFFFFFF, (x * 24) + (NUM_RAYS * RAY_WIDTH), (y * 24), 23, 23);

    square(fb, 0x4F4F4F, 0, fb->height / 2, NUM_RAYS * RAY_WIDTH, fb->height / 2);
    square(fb, 0x73C5FF, 0, 0, NUM_RAYS * RAY_WIDTH, fb->height / 2);

    // Rays
    drawRays(fb);

    // Player
    float drawPX = ((px / MAP_BLOCK_SIZE) * 24) + (NUM_RAYS * RAY_WIDTH);
    float drawPY = ((py / MAP_BLOCK_SIZE) * 24);
    square(fb, 0xFFFF00, drawPX - 8, drawPY - 8, 16, 16);
    // line(fb, 0xFF00FF, drawPX + 8, drawPY + 8, (drawPX + (pdx * 5)) + 8, (drawPY + (pdy * 5)) + 8);
}

int main(int argc, char **argv)
{
    FILE *textures = fopen("TEXT.TGA");
    if (textures == NULL)
        return -1;

    texture_data = tga_parse(textures->data, textures->sizeSector * 504) + 2;

    pa = 0.f;
    pdx = cos(pa) * 5;
    pdy = sin(pa) * 5;

    fb_init();
    FB_DATA *fb = fb_get();
    while (1)
    {
        // update stuff idk
        update();

        // commence the drawing.
        for (int i = 0; i < fb->height; i++)
            for (int j = 0; j < fb->width; j++)
                fb_putpix(fb, 0, j, i);
        draw(fb);
        fb_flip(fb);
    }

    return 0;
}
