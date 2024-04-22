
[BITS 64]

[GLOBAL _start]
_start:
    mov bl, 'A' ; Character to put on screen
    int 0x80

    mov bl, 0 ; Return code
    int 0x85
    jmp $