
#ifndef __MEDUZAOS_FB
#define __MEDUZAOS_FB

#include <stdint.h>
#include <stddef.h>

typedef struct
{
	void *base_addr;
	size_t size;
	unsigned int width;
	unsigned int height;
	unsigned int pix_per_sc;
} FB_DATA;

uint32_t *tga_parse(unsigned char *ptr, int size);
FB_DATA *fb_get();
void fb_init();
void fb_flip(FB_DATA *fb);
void fb_putpix(FB_DATA *fb, uint32_t color, int x, int y);
uint32_t fb_getpix(FB_DATA *fb, int x, int y);

#endif