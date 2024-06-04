
#include "elf.h"
#include "paging/PageFrameAllocator.h"
#include "paging/PageTableManager.h"
#include "stdio/stdio.h"

int elf_err = ELF_GOOD;

int elf_get_err()
{
    return elf_err;
}

int elf_verify(uint8_t *bytes)
{
    Elf64_Ehdr *header = (Elf64_Ehdr *)bytes;
    if (header->e_ident[0] != 0x7F ||
        header->e_ident[1] != 'E' ||
        header->e_ident[2] != 'L' ||
        header->e_ident[3] != 'F')
        return ELF_BAD_HDR;
    if (header->e_type != ET_EXEC)
        return ELF_NOT_EXEC;

    return ELF_GOOD;
}

void *elf_load(char name[8], uint8_t *bytes)
{
    Elf64_Ehdr *header = (Elf64_Ehdr *)bytes;

    if (header->e_ident[0] != 0x7F ||
        header->e_ident[1] != 'E' ||
        header->e_ident[2] != 'L' ||
        header->e_ident[3] != 'F')
    {
        printf("elf_load: file is not elf\n");
        elf_err = ELF_BAD_HDR;
        return NULL;
    }
    if (header->e_type != ET_EXEC)
    {
        printf("elf_load: elf is not executable.\n");
        elf_err = ELF_NOT_EXEC;
        return NULL;
    }

    uint64_t prog_entry = header->e_entry;
    g_PageTableManager.MapMemory((void *)prog_entry, (void *)prog_entry);
    for (uint64_t i = 0; i < (uint64_t)header->e_shentsize * 8; i += header->e_shentsize)
    {
        Elf64_Shdr *shdr = (Elf64_Shdr *)((uint64_t)header + (header->e_shoff + i));
        if (shdr->sh_addr == 0) continue;

        g_PageTableManager.MapMemory((void *)shdr->sh_addr, (void *)shdr->sh_addr);

        if (shdr->sh_type == SHT_NOBITS)
            memset((char *)(shdr->sh_addr), 0x0, shdr->sh_size);
        else if (shdr->sh_addr != 0)
            memcpy8((char *)(shdr->sh_addr), (char *)((uint64_t)header + shdr->sh_offset), shdr->sh_size);
    }

    //elf_program *out = (elf_program *)malloc(sizeof(elf_program));
    //memcpy8(out->name, name, 8);
    //out->name[8] = '\0';
    //out->entryPoint = (uint64_t *)prog_entry;
    return (void *)prog_entry;
}