
#include "MeduzaFS.h"
#include "vfs.h"
#include "../cstr.h"

int filedata_offset = 300;
int dirinfo_offset = 17;

DIRINFO *rootDir;
DIRINFO *curDir;

FILEINFO *filesInfo;
DIRINFO *dirsInfo;
SUPERBLOCK *superBlock;

int dirNum = 0;

bool mfsCheckFormat()
{
    if (superBlock->signature[0] == 'M' &&
        superBlock->signature[1] == 'D' &&
        superBlock->signature[2] == 'F' &&
        superBlock->signature[3] == 'S' &&
        superBlock->magic == 0x9F018F02)
        return true;
    return false;
}

void mfsListCurDir()
{
    for (int i = 0; i < dirNum; i++)
    {
        if (dirsInfo[i + 1].parentDirId.val == curDir->id.val)
        {
            printf("%oC");
            for (int j = 0; j < 8; j++)
                printf("%c", dirsInfo[i + 1].dirname[j]);
            printf("%oF\n");
        }
    }

    for (int i = 0; i < superBlock->numFiles; i++)
    {
        if (filesInfo[i].dirId.val == curDir->id.val)
        {
            // printf("%oF");
            for (int j = 0; j < 8; j++)
                printf("%c", filesInfo[i].filename[j]);
            printf("%oF\n");
        }
    }
}

uint24_t dirNameToID(char dirname[8])
{
    uint24_t ID;
    ID.val = -1;
    for (int i = 0; i < superBlock->numFiles; i++)
    {
        if (!strncmp((const char *)dirname, dirsInfo[i].dirname, 8) &&
            dirsInfo[i + 1].parentDirId.val == curDir->id.val)
        {
            ID.val = i;
            break;
        }
    }
    return ID;
}

bool mfsChgDir(char *path)
{
    if (path[0] == '/')
        memcpy(curDir, rootDir, sizeof(DIRINFO));

    char *tok = strtok(path, "/");

    while (tok != NULL)
    {
        if (strcmp(tok, ".."))
        {
            if (curDir->parentDirId.val == 0) // Parent directory is root
                memcpy(curDir, rootDir, sizeof(DIRINFO));
            else
            {
                DIRINFO directory = dirsInfo[curDir->parentDirId.val];

                for (int i = 0; i < 8; i++)
                    curDir->dirname[i] = directory.dirname[i];
                curDir->id.val = directory.id.val;
                curDir->parentDirId.val = directory.parentDirId.val;
                curDir->dirSig = directory.dirSig;
                curDir->magic = directory.magic;
            }
            tok = strtok(NULL, "/");
            continue;
        }
        else if (strcmp(tok, "."))
        {
            tok = strtok(NULL, "/");
            continue;
        }

        uint24_t dirID = dirNameToID(tok);
        if (dirID.val < 0 || dirID.val > dirNum)
            return 1;

        DIRINFO directory = dirsInfo[dirID.val];

        for (int i = 0; i < 8; i++)
            curDir->dirname[i] = directory.dirname[i];
        curDir->id.val = directory.id.val;
        curDir->parentDirId.val = directory.parentDirId.val;
        curDir->dirSig = directory.dirSig;
        curDir->magic = directory.magic;
        tok = strtok(NULL, "/");
    }

    return 0; // everything went alright
}

uint32_t fileNameToID(char filename[8])
{
    uint32_t ID = -1;
    for (int i = 0; i < superBlock->numFiles; i++)
    {
        if (!strncmp((const char *)filename, filesInfo[i].filename, 8) &&
            filesInfo[i].dirId.val == curDir->id.val)
        {
            ID = i;
            break;
        }
    }
    return ID;
}

FILE *mfsOpenFile(char *path)
{
    char filename[8];
    for (int i = 0; i < 8; i++)
        filename[i] = path[i + (strlen(path) - 8)];
    
    // this is a very bad idea
    path[strlen(path) - 8] = 0;

    DIRINFO *prevDir = (DIRINFO *)malloc(sizeof(DIRINFO));
    for (int i = 0; i < 8; i++)
        prevDir->dirname[i] = curDir->dirname[i];
    prevDir->dirSig = curDir->dirSig;
    prevDir->id = curDir->id;
    prevDir->magic = curDir->magic;
    prevDir->parentDirId = curDir->parentDirId;

    mfsChgDir(path);

    uint32_t id = fileNameToID(filename);
    if (id == -1)
        return NULL;
    
    for (int i = 0; i < 8; i++)
        curDir->dirname[i] = prevDir->dirname[i];
    curDir->dirSig = prevDir->dirSig;
    curDir->id = prevDir->id;
    curDir->magic = prevDir->magic;
    curDir->parentDirId = prevDir->parentDirId;

    free(prevDir);

    AHCI::Port *port = g_ahciDriver->ports[0];

    int i = 0;
    int size = 0;

    while (true)
    {
        vfsRead(filedata_offset + i, 2, port->buffer);
        FILE_DATABLOCK *dataBlock = (FILE_DATABLOCK *)port->buffer;

        if (dataBlock->magic == 0x9F8F)
        {
            if (dataBlock->id == id)
                size++;
        }
        else
            break;
        i++;
    }

    i = 0;
    int extraction = 0;
    uint8_t *data = (uint8_t *)malloc((size * 504));
    while (true)
    {
        vfsRead(filedata_offset + i, 2, port->buffer);
        FILE_DATABLOCK *dataBlock = (FILE_DATABLOCK *)port->buffer;

        if (dataBlock->magic == 0x9F8F)
        {
            if (dataBlock->id == id)
            {
                int j;
                for (j = 0; j < 504; j++)
                    data[j + extraction] = dataBlock->data[j];
                extraction += j;
            }
        }
        else
            break;
        i++;
    }

    FILE *ret = (FILE *)malloc(sizeof(FILE));
    ret->data = (uint8_t *)malloc((size * 504));

    for (i = 0; i < 8; i++)
        ret->filename[i] = filesInfo[id].filename[i];
    ret->filename[8] = '\0';

    for (i = 0; i < size * 504; i++)
        ret->data[i] = data[i];

    ret->magic = 0x9F018F02;
    ret->sizeSector = size;

    free(data);

    return ret;
}

void mfsCloseFile(FILE *file)
{
    if (file != NULL)
    {
        free(file->data);
        free(file);
        // GlobalAllocator.FreePage(file);
    }
    else
        printf("File is NULL, cannot close\n");
}

void mfsInit()
{
    AHCI::Port *port = g_ahciDriver->ports[0];
    superBlock = (SUPERBLOCK *)GlobalAllocator.RequestPage();
    vfsRead(SUPERBLOCK_OFFSET, 1, superBlock);

    if (!mfsCheckFormat())
    {
        printf("[%o1ERROR%oF]: Drive is not Meduza%c FS (MFS) format or drive is corrupted. Halting...\n", 0xA9);
        asm("cli");
        while (1)
            asm("hlt");
    }

    filedata_offset += (superBlock->numFiles * 16) / 512;
    dirinfo_offset += (superBlock->numFiles * 16) / 512;

    filesInfo = (FILEINFO *)malloc(sizeof(FILEINFO) * superBlock->numFiles);
    dirsInfo = (DIRINFO *)malloc(sizeof(DIRINFO) * (superBlock->numFiles + 1));

    curDir = (DIRINFO *)malloc(sizeof(DIRINFO));

    rootDir = (DIRINFO *)malloc(sizeof(DIRINFO));
    for (int i = 0; i < 8; i++)
        rootDir->dirname[i] = "ROOT\0   "[i];
    rootDir->id.val = 0;
    rootDir->parentDirId.val = 0;
    rootDir->dirSig = 'D';
    rootDir->magic = 0x4D;

    memcpy(curDir, rootDir, sizeof(DIRINFO));

    for (int k = 0; k < filedata_offset - FILEINFO_OFFSET; k++)
    {
        int exit = 0;
        vfsRead(FILEINFO_OFFSET + k, 1, port->buffer);
        FILEINFO *fileInfo = (FILEINFO *)port->buffer;

        for (int i = 0; i < 32; i++)
        {
            if (i != 0)
                fileInfo += 1;
            if (fileInfo->magic == 0x4D)
            {
                for (int j = 0; j < 8; j++)
                    filesInfo[i].filename[j] = fileInfo->filename[j];
                filesInfo[i].id = fileInfo->id;
                filesInfo[i].magic = fileInfo->magic;
                filesInfo[i].dirId.val = fileInfo->dirId.val;
            }
            else
            {
                exit = 1;
                break; // Break because we know there aren't any files beyond
            }
        }
        if (exit)
            break;
    }

    for (int k = 0; k < dirinfo_offset - FILEINFO_OFFSET; k++)
    {
        int exit = 0;
        vfsRead(dirinfo_offset + k, 1, port->buffer);
        DIRINFO *dirInfo = (DIRINFO *)port->buffer;
        for (int i = 0; i < 32; i++)
        {
            if (i != 0)
                dirInfo += 1;
            if (dirInfo->magic == 0x4D)
            {
                dirNum++;
                for (int j = 0; j < 8; j++)
                    dirsInfo[i + 1].dirname[j] = dirInfo->dirname[j];
                dirsInfo[i + 1].id = dirInfo->id;
                dirsInfo[i + 1].magic = dirInfo->magic;
                dirsInfo[i + 1].parentDirId.val = dirInfo->parentDirId.val;
            }
            else
            {
                exit = 1;
                break; // Break because we know there aren't any files beyond
            }
        }
        if (exit)
            break;
    }
}