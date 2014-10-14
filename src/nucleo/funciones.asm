%include "../include/sodero/syscalls.inc"

%define MEM_VIDEO 0xB8000

%define CANT_COLUMNAS 80
%define CANT_FILAS 25

;%define er 'E'
;%define bien 'B'
;
;   --------------  <-- EBP + 12 ( direccion del segundo parametro)
;  |  parametro2  |
;  |  (4 bytes)   |
;   --------------  <-- EBP + 8 ( direccion del primer parametro)
;  |  parametro1  |
;  |  (4 bytes)   |
;   --------------  
;  | direccion de |
;  |   retorno    |
;   --------------  <-- aca apunta el ESP cuando inicio la subrutina	
;  | valor de EBP |     inmediatamente se hace un push de ebp con lo que
;  |  (4 bytes)   |     el valor del stack de decrementa.
;   --------------  <-- aca hago apuntar al EBP para luego
;  |variable local|     referenciar por intermedio suyo a los argumentos
;  |  (4 bytes)   |     
;   -------------- 
;  |otra variable |
;  |  (2 bytes)   |
;   --------------  <-- aca apunta el ESP cuando le resto 6 bytes
;  |              |     para declarar variables locales a la subrutina
;  |              |     entonces asi poder usar el stack para push y pop
;  |              |     sin que me interfiera en nada!!
;
; En este caso particular necesitaremos en el stack declarar dos variables
; locales a la subrutina: los primeros 4 bytes correspondientes a la base
; de la IDT y los siguientes 2 bytes a su tamanio (recordar que intel es
; litle endian, con lo cual al referenciar a [EBP-6] vamos a apuntar a
; la segunda variable (limite) y dos bytes a continuacion de esta a la
; primera (base), teniendo de esta forma declarada la tabla que
; representa a la IDT, es decir, si hacemos un lidt [ebp-6] le estamos
; pasando los datos que necesitamos para cargar la tabla en el idtr

global cargar_idt
global llamar_int
global bucle_infinito
global habilitar_irqs

global sys_obtener_y
global sys_obtener_x
global sys_obtener_color
global sys_setear_y
global sys_setear_x
global sys_setear_color
global sys_obtener_memoria_absoluta

global irq0_handler_asm
global irq1_handler_asm
global irq2_handler_asm
global irq3_handler_asm
global irq4_handler_asm
global irq5_handler_asm
global irq6_handler_asm
global irq7_handler_asm
global irq8_handler_asm
global irq9_handler_asm
global irq10_handler_asm
global irq11_handler_asm
global irq12_handler_asm
global irq13_handler_asm
global irq14_handler_asm
global irq15_handler_asm
global sys_call_handler_asm
global sys_call_handler

global sys_printc_asm

extern imprimir

extern timer_handler
extern teclado_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern disquetera_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler

extern tabla_sys_calls

segment data
	pos_x	dd	0
	pos_y	dd	4
	color	db	7

segment data
%ifdef DEBUG
msg	dw	"En cargar_idt: IDTR->base=0x%xw - IDT->limite=0x%x",10,13,0
%endif
cadena	dw	"Sali de imprimir INT40",10,13,0

ALIGN 4
cargar_idt:
	push	ebp		; almaceno el valor actual de ebp
	mov	ebp, esp	; copio esp en el ebp
	sub	esp, 6		; le resto a esp la cantidad de bytes que voy
				; a utilizar para declarar variables en el
				; stack (ver nota mas arriba)

	mov	eax, [ebp + 8]	; [ebp+8] es el primer argumento que recibo
				; (en este caso la direccion de la base)

	mov	dword [ebp - 4], eax	; muevo la base de la idt a la
					; posicion en memoria reservada para
					; eso (primer variable en el stack)
	xor	eax, eax
	mov	eax, [ebp + 12]	; el segundo argumento sera el tamanio
				; de la idt.  NOTA: por mas que necesitemos
				; solo 2 bytes, el compilador de C reservara
				; 4 bytes para este argumento tambien,
				; dejando los 2 bytes restantes "libres"

	mov	word [ebp - 6], ax	; que es movido a la segunda
					; variable privada a la subrutina
					; que declaramos en el stack
					; (solo se mueve AX porque son 2 bytes)

	lidt	[ebp - 6]		; cargamos la idt con los valores
					; que acabamos de setear....

	%ifdef DEBUG
	xor	eax, eax
	mov	ax, [ebp - 6]
	push	eax

	mov	eax, [ebp - 4]
	push	eax

	push	dword msg
	call	imprimir
	%endif

	mov	esp, ebp		; restauro a esp en su posicion
					; original para que pueda leer
					; la direccion de retorno de la
					; subrutina

	pop	ebp			; recupero a ebp
	ret

llamar_int
	int	0x20
	ret

bucle_infinito:
	jmp $


habilitar_irqs:
	push	eax

	mov	al, 0x11		; secuencia de inicializacion
	out	0x20, al		; enviarsela al 8259A-1
	jmp	short $+2
	out	0xA0, al		; y tambien al 8659A-2
	jmp	short $+2
	mov	al, 0x20		; las IRQs comienzan en 0x20
					; (desde IRQ0-7)
	out	0x21, al
	jmp	short $+2
	mov	al, 0x28		; las IRQs comienzan en 0x28
					; (desde IRQ8-15)
	out	0xA1, al
	jmp	short $+2
	mov	al, 0x04		; el 8259A-1 es el PIC maestro
	out	0x21, al
	jmp	short $+2
	mov	al, 0x02		; el 8259A-2 es el PIC esclavo
	out	0xA1, al
	jmp	short $+2
	mov	al, 0x01		; ambos PIC estan en modo 8086
	out	0x21, al
	jmp	short $+2
	out	0xA1, al
	jmp	short $+2
	;mov	al, 0xFF		; enmascarar las interrupciones
	mov	al, 0x00
	out	0x21, al
	jmp	short $+2
	out	0xA1, al

	pop	eax

	ret


irq0_handler_asm:
.loop
	push	dword 0x0
	call	timer_handler
	add	esp, byte 4
	iret
jmp	.loop
	
irq1_handler_asm:
.loop
	push	dword 0x1
	call	teclado_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq2_handler_asm:
.loop
	push	dword 0x2
	call	irq2_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq3_handler_asm:
.loop
	push	dword 0x3
	call	irq3_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq4_handler_asm:
.loop
	push	dword 0x4
	call	irq4_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq5_handler_asm:
.loop
	push	dword 0x5
	call	irq5_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq6_handler_asm:
.loop
	push	dword 0x6
	call	disquetera_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq7_handler_asm:
.loop
	push	dword 0x7
	call	irq7_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq8_handler_asm:
.loop
	push	dword 0x8
	call	irq8_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq9_handler_asm:
.loop
	push	dword 0x9
	call	irq9_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq10_handler_asm:
.loop
	push	dword 0x10
	call	irq10_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq11_handler_asm:
.loop
	push	dword 0x11
	call	irq11_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq12_handler_asm:
.loop
	push	dword 0x12
	call	irq12_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq13_handler_asm:
.loop
	push	dword 0x13
	call	irq13_handler
	add	esp, byte 4
	iret
jmp	.loop
		
irq14_handler_asm:
.loop
	push	dword 0x14
	call	irq14_handler
	add	esp, byte 4
	iret
jmp	.loop
			
irq15_handler_asm:
.loop
	push	dword 0x15
	call	irq15_handler
	add	esp, byte 4
	iret
jmp	.loop
		
sys_call_handler_asm:
	push    ds
	push    word 0x10
	pop     ds

	push	eax
	push	ebx
	push	ecx
	push	edx

	call	sys_call_handler

        push    dword cadena
        call    imprimir

	pop		ds
	;add		esp, byte 4
	iret

;/*************************************************************************
; * sys_call_handler: punto de entrada para las system calls (handler de la
; *                   interrupcion 0x40.
; * Argumentos:
; *      - eax: numero de la sys call
; *      - ebx: primer argumento de la sys call ( si lo tiene )
; *      - ecx: segundo argumento de la sys call ( si lo tiene )
; *      - edx: tercer argumento de la sys call ( si lo tiene )
; *************************************************************************/

ALIGN 4
sys_call_handler:

	cmp	eax, CANT_SYS_CALLS
	jge	.error   ;verifica validez del subindice

	push	ds
	push	es
	push	fs
	push	gs

	push	eax
	mov	eax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	pop	eax

	push	edx
	push	ecx
	push	ebx
	call	[tabla_sys_calls + 4 * eax]
	add	esp, 12

	pop	gs
	pop	fs
	pop	es
	pop	ds

        ;push    ebx
        ;mov     byte bl, bien 
        ;call    sys_printc_asm
        ;pop     ebx
	jmp	.ok

.error    ;imprimo un caracter 'E' por ahi...
        ;push    ebx
        ;mov     byte bl, er 
        ;call    sys_printc_asm
        ;pop     ebx
	mov	eax, -1
.ok
	iret

ALIGN 4
sys_printc_asm
	push	ebp
	mov	ebp, esp

	push	ds
	push	eax
	mov	eax, 0x10
	mov	ds, ax

	push	edx
	push	ecx
	push	ebx

	;((dword)(MEM_VIDEO + (160 * pos_y + pos_x * 2)))
	xor	eax, eax
	xor 	edx, edx

	mov	al, [pos_y]
	imul	ax, 160		; eax = ax * 160; / edx:eax = eax * 160;

	mov	dl, [pos_x]
	shl	dl, 1

	add	edx, eax
	add	edx, MEM_VIDEO

	pop	eax

	mov	ebx, [ebp+8]
	mov	ecx, [color]

	mov	byte [edx], bl
	mov	byte [edx+1], cl

	pop	ebx
	pop	ecx
	pop	edx

	pop	ds

	mov	esp, ebp
	pop	ebp
	
	ret

ALIGN 4
sys_setear_y
	push	ebp
	mov	ebp, esp

	mov	eax, [ebp + 8]
	mov	[pos_y], eax

	mov	esp, ebp
	pop	ebp
	
	ret

ALIGN 4
sys_setear_x
	push	ebp
	mov	ebp, esp

	mov	eax, [ebp + 8]
	mov	[pos_x], eax

	mov	esp, ebp
	pop	ebp
	
	ret

ALIGN 4
sys_obtener_color
	push	ebp
	mov	ebp, esp

	mov	eax, [color]

	mov	esp, ebp
	pop	ebp
	
	ret

ALIGN 4
sys_setear_color
	push	ebp
	mov	ebp, esp

	mov	eax, [ebp + 8]
	mov	[color], eax

	mov	esp, ebp
	pop	ebp
	
	ret

ALIGN 4
sys_obtener_y
	push	ebp
	mov	ebp, esp

	mov	eax, [pos_y]

	mov	esp, ebp
	pop	ebp
	
	ret

ALIGN 4
sys_obtener_x
	push	ebp
	mov	ebp, esp

	mov	eax, [pos_x]

	mov	esp, ebp
	pop	ebp
	
	ret
