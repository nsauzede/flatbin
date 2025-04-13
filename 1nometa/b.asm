BITS 64

    org 0x10000

    mov eax, 1                  ; rax = 1: write system call
    mov edi, eax                ; rdi = 1: stdout file desc
    lea rsi, [rel str]          ; rsi = pointer to string
    mov edx, strlen             ; rdx = string length
    syscall                             ; call write(rdi, rsi, rdx)
    mov eax, 60                 ; rax = 60: exit system call
    xor edi, edi                ; rdi = 0: exit code
    syscall                             ; call exit(rdi)

str:    db      'hello, world', 10
strlen equ $ - str
