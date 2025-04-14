;; env.asm: output the contents of envp, one entry per line.

BITS 64

	org	0x10000

	; sp+0: return address
	; sp+8: argc
	; sp+16:argv
	; sp+24:envp
	mov     rbx, [rsp + 24]  ; rbx = envp array

varloop:
	xor	eax, eax
	mov	rdi, [rbx]		; end of envp?
	or	rdi, rdi
	jz	done			; yes, exit
	mov	rsi, rdi		; esi = env string
	mov	ecx, -1
	repnz scasb			; find end of string
	mov	edx, -2
	sub	edx, ecx		; edx = string length
	mov	edi, 1
	mov	eax, edi
	syscall				; write(1, esi, edx)
	lea	rsi, [rel NL]
	mov	edx, edi
	mov	eax, edi
	syscall				; write(1, "\n", 1)
	add	rbx, 8
	jmp	short varloop		; loop until end of vars

NL:	db	10			; newline byte

done:	; eax=0 already
	ret
