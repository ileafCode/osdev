
#include "vfs.h"
#include "../memory/heap.h"
#include "../paging/PageFrameAllocator.h"
#include "../stdio/stdio.h"

bool VFS_DISK::Read(uint64_t sector, uint32_t sectorCount, void *buffer)
{
    if (this->name.portIdx != -1)
    {
        g_ahciDriver->ports[this->name.portIdx]->Read(sector, sectorCount, buffer);
        return 0;
    }
    else
    {
        printf("[%o9ERROR%oF]: { VFS_DISK::Read(%d, %d, %x); } Disk is not mounted\n", sector, sectorCount, (uint64_t *)buffer);
        return 1; // Error
    }
}

bool VFS_DISK::Write(uint64_t sector, uint32_t sectorCount, void *buffer)
{
    if (this->name.portIdx != -1)
    {
        g_ahciDriver->ports[this->name.portIdx]->Write(sector, sectorCount, buffer);
        return 0;
    }
    else
    {
        printf("[%o9ERROR%oF]: { VFS_DISK::Write(%d, %d, %x); } Disk is not mounted\n", sector, sectorCount, (uint64_t *)buffer);
        return 1; // Error
    }
}

VFS_DISK *vfs_disk;
int drvNameIdx = 0;
VFS_NAME names[26];

void vfsAddDisk(char name)
{
    if (drvNameIdx > 26)
    {
        printf("[%o9ERROR%oF]: { vfsAddDisk(%c); } Too many drives\n", name);
        return;
    }
    names[drvNameIdx].name = name;
    names[drvNameIdx].portIdx = drvNameIdx;
    drvNameIdx++;
    // printf("Drive name: %c\n", name);
}

void vfsMount(char name)
{
    for (int i = 0; i < drvNameIdx; i++)
    {
        if (names[i].name == name)
        {
            vfs_disk->name.name = names[i].name;
            vfs_disk->name.portIdx = names[i].portIdx;
            return;
        }
    }
    printf("[%o9ERROR%oF]: { vfsMount(%c); } Could not find drive\n", name);
}

void vfsUnmount()
{
    vfs_disk->name.portIdx = -1;
}

bool vfsWrite(uint64_t sector, uint32_t sectorCount, void *buffer)
{
    return vfs_disk->Write(sector, sectorCount, buffer);
}

bool vfsRead(uint64_t sector, uint32_t sectorCount, void *buffer)
{
    return vfs_disk->Read(sector, sectorCount, buffer);
}