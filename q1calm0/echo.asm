;; echo.asm: output the contents of argv, separated by spaces.

BITS 64

    org 0x10000

    pop rdi                     ; rdi = return address
    pop rbp                     ; rbp = argc
    pop rbx                     ; rbx = argv
    push rdi                    ; restore return address
    mov edi, 1                  ; rdi = 1 (constant)

    dec rbp                     ; argc <= 1?
    jle done                    ; yes, quit now

argloop:
    add rbx, 8                  ; advance to next arg
    mov rsi, [rbx]
len:    lodsb
    or  al, al
    jnz len                     ; find end of string
    mov rdx, rsi
    mov rsi, [rbx]              ; rsi = arg pointer
    sub rdx, rsi
    dec rdx                     ; rdx = string length
    mov eax, edi
    syscall                             ; write(1, rsi, rdx)
    dec rbp                     ; any args left?
    jle done                    ; no, exit loop here
    lea rsi, [rel SPC]
    mov edx, edi
    mov eax, edi
    syscall                             ; write(1, " ", 1)
    jmp argloop                 ; and repeat

done:
    lea rsi, [rel NL]
    mov edx, edi
    mov eax, edi
    syscall                             ; write(1, "\n", 1)
    xor eax,eax
    ret

SPC:    db      32                      ; space character
NL:     db      10                      ; line break
