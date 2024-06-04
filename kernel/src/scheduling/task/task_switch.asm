section .text
[BITS 64]

%macro pushaq 0
    push rax
    push rcx
    push rdx
    push rbx
    push rsp
    push rbp
    push rsi
    push rdi
%endmacro

%macro popaq 0
    pop rdi
    pop rsi
    pop rbp
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

[GLOBAL switchTask]
switchTask:
    pop r14 ; Pop into r14. Now r14 has the address of RIP before calling switchTask.

    ; Save current task registers
    mov qword [rdi],      rax
    mov qword [rdi + 8],  rbx
    mov qword [rdi + 16], rcx
    mov qword [rdi + 24], rdx
    mov qword [rdi + 32], rsi
    mov qword [rdi + 40], rdi
    mov qword [rdi + 48], rsp
    mov qword [rdi + 56], rbp
    mov qword [rdi + 64], r8
    mov qword [rdi + 72], r14

    pushfq
    pop qword [rdi + 80] ; Saving flags register (RFLAGS)

    ; Restore other task registers
    mov rax, qword [rsi]
    mov rbx, qword [rsi + 8]
    mov rcx, qword [rsi + 16]
    mov rdx, qword [rsi + 24]
    mov rdi, qword [rsi + 40]
    mov rbp, qword [rsi + 56]
    mov r15, qword [rsi + 72]
    mov r8,  qword [rsi + 64]
    push qword [rsi + 80] ; Restoring RFLAGS
    popfq

    mov rbp, qword [rsi + 56]
    mov rsp, qword [rsi + 48]
    mov rsi, qword [rsi + 32]

    sti
    push r15
    ret
