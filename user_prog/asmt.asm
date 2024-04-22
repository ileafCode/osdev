
[BITS 64]

_start:
    mov bl, 'A'
    int 0x80
    jmp $
    ret