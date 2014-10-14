global delay
extern imprimir

;/**
; * delay: Funcion que se encarga de realizar un retardo de tiempo en segundos.
; *
; * @param tiempo_a_retardar Recibe el tiempo a realizar el retardo en segundos.
; */
ALIGN 4
delay:
	push	ebp		; almaceno el valor actual de ebp
	mov	ebp, esp	; copio esp en el ebp

	push	eax
	push	ecx
	push	edx

	mov	ecx, [ebp + 8]	; recupero el primer argumento

	jcxz	.fin_l
.esp_ref:
	in	al, 0x61    ;leo sobre el puerto 0x61
	and	al, 0x10
	cmp	al, ah
	je	.esp_ref
	mov	ah, al
	loop	.esp_ref    ;realizo bucle hasta condicion de fin
.fin_l:
	and	cx, cx
	jz	.fin_ret
	dec	ecx
	jmp	.esp_ref
.fin_ret:
	pop	edx
	pop	ecx
	pop	eax

	mov	esp, ebp		; restauro a esp en su posicion
					; original para que pueda leer
					; la direccion de retorno de la
					; subrutina

	pop	ebp			; recupero a ebp
	ret
