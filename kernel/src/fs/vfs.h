
#pragma once
#include <stdint.h>
#include "../ahci/ahci.h"

struct VFS_NAME
{
    char name;
    int portIdx = -1;
};

struct VFS_DISK
{
    VFS_NAME name;
    bool Write(uint64_t sector, uint32_t sectorCount, void *buffer);
    bool Read(uint64_t sector, uint32_t sectorCount, void *buffer);
};

extern VFS_DISK* vfs_disk;

void vfsAddDisk(char name);
void vfsMount(char name);
void vfsUnmount();
bool vfsWrite(uint64_t sector, uint32_t sectorCount, void *buffer);
bool vfsRead(uint64_t sector, uint32_t sectorCount, void *buffer);