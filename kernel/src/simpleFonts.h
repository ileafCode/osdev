#pragma once

#define PSF2_MAGIC0 0x72
#define PSF2_MAGIC1 0xb5
#define PSF2_MAGIC2 0x4a
#define PSF2_MAGIC3 0x86

typedef struct
{
	unsigned char magic[4];
	unsigned int version;
	unsigned int headersize;
	unsigned int flags;
	unsigned int length;
	unsigned int charsize;
	unsigned int height, width;
} PSF2_HEADER;

typedef struct
{
	PSF2_HEADER *psf2_header;
	void *glyph_buffer;
} PSF2_FONT;

struct PSF1_HEADER
{
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
};

struct PSF1_FONT
{
	PSF1_HEADER *psf1_Header;
	void *glyphBuffer;
};