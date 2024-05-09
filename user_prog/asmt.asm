
; Basic MeduzaOS program

[BITS 64]

[GLOBAL _start]
_start:
    mov rbx, 0 ; Return code
    int 0x85
    jmp $ ; Just in case