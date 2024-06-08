
#ifndef __MEDUZAOS_MEDUZA
#define __MEDUZAOS_MEDUZA

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint32_t fsize;          /* File size */
    uint16_t fdate;          /* Modified date */
    uint16_t ftime;          /* Modified time */
    uint8_t fattrib;         /* File attribute */
    uint8_t altname[12 + 1]; /* Alternative file name */
    uint8_t fname[255 + 1];  /* Primary file name */
} FILINFO;

typedef struct
{
    uint8_t fs[24];   /* Object identifier */
    uint32_t dptr;    /* Current read/write offset */
    uint32_t clust;   /* Current cluster */
    uint32_t sect;    /* Current sector (0:Read operation has terminated) */
    uint8_t *dir;     /* Pointer to the directory item in the win[] */
    uint8_t fn[12];   /* SFN (in/out) {body[8],ext[3],status[1]} */
    uint32_t blk_ofs; /* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
} DIR;

/*
    Sleeps for ms amount of milliseconds.
*/
void sleep(uint64_t ms);

int opendir(DIR *dir, const char *path);
int closedir(DIR *dir);
int readdir(DIR *dir, FILINFO *finfo);

#endif