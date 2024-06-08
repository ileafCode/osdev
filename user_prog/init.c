
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/meduza.h>

int list_dir(const char *path)
{
    int res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    res = opendir(&dir, path);
    if (res == 0)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = readdir(&dir, &fno);
            if (res != 0 || fno.fname[0] == 0)
                break;
            if (fno.fattrib & 0x10)
            {
                printf("\033[0;91m%s\033[0m ", fno.fname);
                ndir++;
            }
            else
            {
                printf("%s ", fno.fname, fno.fsize);
                nfile++;
            }
        }
        closedir(&dir);
    }
    else
    {
        printf("[ERROR]: Failed to open \"%s\". (%u)\n", path, res);
    }
    return res;
}

int main(void)
{
    list_dir("/bin/");
    return 0;
}