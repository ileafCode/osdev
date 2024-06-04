
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/kbd.h>

int main(int argc, char **argv)
{
    FILE *file = fopen("test.txt", "r");
    if (file == NULL)
        return 1;
    
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buf = (char *)malloc(fsize);

    fread(buf, 1, fsize, file);

    for (int i = 0; i < fsize; i++)
    {
        syscall(SYSCALL_writeConsole, buf[i], 0, 0);
    }

    free(buf);

    fclose(file);
    return 0;
}