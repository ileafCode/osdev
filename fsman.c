
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

int filedata_offset = 300;
#define FILEINFO_OFFSET 16
#define SUPERBLOCK_OFFSET 15

typedef struct
{
    uint32_t val : 24;
} __attribute__((packed)) uint24_t;

struct SUPERBLOCK
{
    uint32_t numFiles;
    uint32_t numDirs;
    char signature[4]; // Must be 'MDFS'
    uint32_t driveSize;
    uint32_t magic; // Must be 0x9F018F02
} __attribute__((packed));

struct MFS_FILE
{
    char filename[8];
    uint32_t id;
    uint8_t magic; // Must be 0x4D
    uint24_t dirId;
} __attribute__((packed));

struct DIRINFO
{
    char dirname[8];
    uint24_t id;
    uint8_t magic; // Must be 0x4D
    char dirSig; // Must be 'D'
    uint24_t dirId;
} __attribute__((packed));

struct DATABLOCK
{
    uint32_t id;
    uint16_t index;
    uint16_t magic;
    uint8_t data[504];
} __attribute__((packed));

int main(int argc, char **argv)
{
    // SuperBlock
    FILE *imgFile = fopen("./kernel/bin/MeduzaOS.img", "r+");
    if (imgFile == NULL)
    {
        printf("File did not open.\n");
        return -1;
    }

    struct SUPERBLOCK *sBlock = (struct SUPERBLOCK *)calloc(sizeof(struct SUPERBLOCK), 1);
    sBlock->numFiles = (uint32_t)argc - 1;
    sBlock->numDirs = 0;
    sBlock->signature[0] = 'M';
    sBlock->signature[1] = 'D';
    sBlock->signature[2] = 'F';
    sBlock->signature[3] = 'S';
    sBlock->driveSize = 0;
    sBlock->magic = 0x9F018F02;

    filedata_offset += (sBlock->numFiles * 16) / 512;

    fseek(imgFile, SUPERBLOCK_OFFSET * 512, SEEK_SET);
    fwrite(sBlock, sizeof(uint32_t), 20, imgFile);
    
    printf("Successfully written SuperBlock to MeduzaOS.img!\n");

    int lastSize = 0;

    char *buffer = (char *)calloc(512, 1);

    struct DIRINFO *dirInfo = (struct DIRINFO*)calloc(sizeof(struct DIRINFO), 1);
    dirInfo->id.val = 1;
    dirInfo->dirId.val = 0;
    dirInfo->dirSig = 'D';
    dirInfo->magic = 0x4D;
    for (int i = 0; i < 8; i++)
        dirInfo->dirname[i] = "TESTDIR"[i];
    
    fseek(imgFile, (17 + ((sBlock->numFiles * 16) / 512)) * 512, SEEK_SET);
    fwrite(dirInfo, sizeof(uint8_t), 16, imgFile);

    for (int k = 0; k < argc - 1; k++)
    {
        int lengthFilename = strlen(argv[k + 1]);
        char *filenameLast8 = argv[k + 1];

        if (lengthFilename > 8)
            filenameLast8 = argv[k + 1] + (lengthFilename - 8);
        
        // Files
        FILE *testFile = fopen(argv[k + 1], "r");
        if (testFile == NULL)
        {
            printf("%s not found.\n", filenameLast8);
            continue;
        }

        printf("Writing %s...", filenameLast8);

        fseek(testFile, 0, SEEK_END);
        int size = ftell(testFile);
        fseek(testFile, 0, SEEK_SET);

        // File info
        struct MFS_FILE *fileInfo = (struct MFS_FILE *)calloc(sizeof(struct MFS_FILE), 1);
        if (fileInfo == NULL)
        {
            printf("Failed to malloc fileInfo at file %d\n", k);
            return -1;
        }
        fileInfo->id = k;
        fileInfo->magic = 0x4D;
        fileInfo->dirId.val = 0; // Root directory
        for (int i = 0; i < 8; i++)
            fileInfo->filename[i] = filenameLast8[i];

        fseek(imgFile, (FILEINFO_OFFSET * 512) + (16 * k), SEEK_SET);
        fwrite(fileInfo, sizeof(uint8_t), 16, imgFile);

        // File data
        struct DATABLOCK *fileData = (struct DATABLOCK *)calloc(sizeof(struct DATABLOCK), 1);
        if (fileData == NULL)
        {
            printf("Failed to malloc fileData at file %d\n", k);
            return -1;
        }
        fileData->id = k;
        fileData->magic = 0x9F8F;
        fileData->index = 0;

        int sizeSectors = (size / 512);

        int j;
        for (j = 0; j < sizeSectors + 1; j++)
        {
            fseek(testFile, (j * 504), SEEK_SET);
            memset(buffer, 0, 512);
            fread(buffer, sizeof(char), 504, testFile);
            memcpy(fileData->data, buffer, 504);

            fseek(imgFile, ((lastSize) + (filedata_offset + j)) * 512, SEEK_SET);
            fwrite(fileData, 1, 512, imgFile);
        }

        for (j = 0; j < sizeSectors + 1; j++)
        {
            fseek(imgFile, ((lastSize) + (filedata_offset + j)) * 512, SEEK_SET);
            fread(buffer, 1, 512, imgFile);
            if (*(uint16_t*)(buffer + 6) != 0x9F8F)
                printf("Bad magic number in file %d\n", k);
        }

        lastSize += j;

        printf("Done!\n");

        free(fileData);
        free(fileInfo);
        fclose(testFile);
    }

    fclose(imgFile);
    free(sBlock);
    free(buffer);

    return 0;
}