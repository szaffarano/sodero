%include "../include/sodero/syscalls.inc"

global printc

;/**
; * printc: esta funcion se encarga de llamar a la system call que imprime
; * un caracter en pantalla con los parametros necesarios.
; *
; * @param caracter char a imprimir.
; */
ALIGN 4
printc:
	push	ebp
	mov	ebp, esp

	push	eax
	push	ebx

	mov	eax, SYS_PRINTC
	mov	ebx, [ebp + 8]
	
	int	0x40

	pop	ebx
	pop	eax

	mov	esp, ebp
	pop	ebp
	ret
