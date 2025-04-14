BITS 64
SECTION .main
; Nothing needed anymore here because comfile
; kernel module already sets rdi/rsi/rdx & stack up

;pop rax
;	pop	rdi
;	pop	rsi
;	pop	rdx
;push rax
;xor rax,rax
;	call .com_file_entry-$-1
;	mov rdi,rax
;	mov al, 60
;	syscall
;.com_file_entry:
