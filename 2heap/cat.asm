;; cat.asm: read stdin into a buffer, and then dump it to stdout.

BITS 64

	;; Get the address of the heap using the brk system call.

	xor	edi, edi
	mov	eax, 12
	syscall
	mov	rbp, rax
	mov	rbx, rax
	mov	rsi, rax

	;; rbp = start of heap buffer
	;; rbx = end of heap buffer
	;; rsi = end of input string

	;; Read stdin into the buffer, expanding it as needed.

input:
	cmp	rsi, rbx		; is buffer full?
	jl	read			; no, skip to read call
	lea	rdi, [rbx + 8192]	; yes, increase buffer size
	mov	eax, 12
	syscall
	or	rax, rax
	jl	fail
	mov	rbx, rdi
read:	mov	rdx, rbx		; read input into heap memory
	sub	rdx, rsi
	xor	edi, edi
	xor	eax, eax
	syscall
	or	rax, rax
	jl	fail
	lea	rsi, [rsi + rax]
	jnz	input			; loop if not EOF

	;; Dump contents of buffer to stdout.

	mov	rdx, rsi
	mov	rsi, rbp
	sub	rdx, rsi
	mov	edi, 1
	mov	eax, edi
	syscall

	;; Exit the program.

done:	xor	eax, eax		; exit with 0
fail:	xor	edi, edi		; exit with -rax
	sub	rdi, rax
	mov	eax, 60
	syscall
