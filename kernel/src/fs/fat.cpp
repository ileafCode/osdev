
#include "fat.h"
#include "vfs.h"
#include "../ahci/ahci.h"

void fatInit()
{
    FAT_BS* fat_boot = (FAT_BS*)GlobalAllocator.RequestPage();
    memset(fat_boot, 0, 0x1000);
    vfsRead(0, 2, fat_boot);

    FAT_EXTBS_32* fat_boot_ext_32 = (FAT_EXTBS_32*)fat_boot->extended_section;

    int fat_size = (fat_boot->table_size_16 == 0)? fat_boot_ext_32->table_size_32 : fat_boot->table_size_16;
    int root_dir_sectors = ((fat_boot->root_entry_count * 32) + (fat_boot->bytes_per_sector - 1)) / fat_boot->bytes_per_sector;
    int first_data_sector = fat_boot->reserved_sector_count + (fat_boot->table_count * fat_size) + root_dir_sectors;

    int first_root_dir_sector = first_data_sector - root_dir_sectors;
    int root_cluster_32 = fat_boot_ext_32->root_cluster;
    //int first_sector_of_cluster = ((cluster - 2) * fat_boot->sectors_per_cluster) + first_data_sector;

    printf("[%oCFAT%oF]: Drive size: %fMB\n", ((float)fat_boot->total_sectors_32 * 512) / 1000000);
    printf("[%oCFAT%oF]: First data sector: %d\n", first_data_sector);
    printf("[%oCFAT%oF]: FAT initialized\n");
}