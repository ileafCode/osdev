
#pragma once
#include <stdint.h>
#include "../stdio/stdio.h"
#include "../cstr.h"
#include "../ahci/ahci.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"

#define FILEINFO_OFFSET 16
#define SUPERBLOCK_OFFSET 15

struct uint24_t
{
    uint32_t val : 24;
} __attribute__((packed));

struct SUPERBLOCK
{
    uint32_t numFiles;
    uint32_t numDirs;
    char signature[4]; // Must be 'MDFS'
    uint32_t driveSize;
    uint32_t magic; // Must be 0x9F018F02
} __attribute__((packed));

struct FILEINFO
{
    char filename[8];
    uint32_t id; // ID of the file
    uint8_t magic; // Must be 0x4D
    uint24_t dirId; // Parent directory, 0 if root
} __attribute__((packed));

struct DIRINFO
{
    char dirname[8];
    uint24_t id; // ID of the file
    uint8_t magic; // Must be 0x4D
    char dirSig; // Must be 'D'
    uint24_t parentDirId; // 
} __attribute__((packed));

struct FILE_DATABLOCK
{
    uint32_t id;
    uint16_t index;
    uint16_t magic; // Must be 0x9F8F
    uint8_t data[504];
} __attribute__((packed));

struct FILE
{
    // Filename
    char filename[9];
    // Magic
    uint32_t magic; // Must be 0x9F018F02
    // Size in sectors
    uint32_t sizeSector;
    // Data
    uint8_t *data;
};

void mfsListCurDir();
bool mfsChgDir(char *path);
FILE *mfsOpenFile(char *path);
void mfsCloseFile(FILE *file);
void mfsInit();
