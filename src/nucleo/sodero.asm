

                                       ; Indice           G-L DPL
selector_codigo		equ	0x08   ; 0000 0000 0000 1  0  00
selector_datos		equ	0x10   ; 0000 0000 0001 0  0  00
selector_pila		equ	0x10   ; 0000 0000 0001 0  0  00

;/************************************************************************
; * GDTBASE: macro que calcula la direccion base de la GDT en base a los
; *          tamanios del segmento BSS y del nucleo.  Es importante
; *          que dicha tabla este ALINEADA a 4K! para lo cual se le suman
; *          esos cuatro KB y luego con un AND logico se eliminan las
; *          ultimas 12 posiciones (2^12 = 4K)
; *
; * gracias Frank Cornelis!!!
; *
; ************************************************************************/
%define GDTBASE ((KERNEL_SIZE + BSS_SIZE + 4096 - 1) & 0xFFFFF000)
%define IDTBASE ((GDTBASE + (fin_gdt - inicio_gdt) + 4096 - 1) & 0xFFFFF000)
%define INICIO_MEMORIA ( KERNEL_SIZE + BSS_SIZE + (64 * 1024) + (64 * 1024) )

bits 16
org 0x0
	push	cs		; DS = CS
	pop	ds

info_memoria	
	; int 0x12 obtiene la memoria base (por si no son 640 KB)
	int	0x12
	movzx	eax, ax	
	shl	eax, 0xA		; eax *= 1024 //para expresarla en bytes
	; se almacena en una variable (declarada en main.c) el valor
	; obtenido
a32	mov	[dword LOW_MEMORY_SIZE_ADDR + kernel], eax

	; El servicio 0x88 de la int 15 nos da cuanta memoria tiene
	; la PC (en KB) -> memoria extendida
	; Primero se setea en cero por si no es posible obtener dicho dato
a32	mov	dword [dword BIOS_MEMORY_SIZE_ADDR + kernel], 0
   	mov	ah,88h
        int	15h
	jc	.no_soportado
		
	; se multiplica por 1024 para expresarla en bytes
	movzx	eax, ax	
	shl	eax,0xA	         ; multiplica por 1024
	add	eax,0x100000     ; suma 1 mb
	; se almacena el valor en la variable correspondiente
	; (declarada en main.c)
a32	mov	[dword BIOS_MEMORY_SIZE_ADDR + kernel], eax

.no_soportado
	cli			; deshabilitar interrupciones

; habilitar la compurta A20 (gracias Linus Torvalds!)
; extraido de linux version 0.0.1
	call    Empty8042
	mov     al,0D1h
	out     64h,al
	call    Empty8042
	mov     al,0DFh
	out     60h,al
	call    Empty8042

; setear el stack (al final de la memoria base)
a32	mov	eax, [dword LOW_MEMORY_SIZE_ADDR + kernel]
	mov	[stack_pointer], eax

	push	cs			; DS = CS
	pop	ds	

	push	si
	mov	si, loader_bienvenida
	call    imprimir
	pop	si

; se almacenan en las variables declaradas en el kernel
; los respectivos tamanios del kernel y el segmento BSS
a32	mov	dword [dword KERNEL_SIZE_ADDR + kernel], KERNEL_SIZE
a32	mov	dword [dword BSS_SIZE_ADDR + kernel], BSS_SIZE
a32	mov	dword [dword IDT_ADDR + kernel], IDTBASE
a32	mov	dword [dword GDT_ADDR + kernel], GDTBASE
a32	mov	dword [dword INICIO_MEMORIA_ADDR + kernel], INICIO_MEMORIA

	
; se copia el codigo del loader desde 0x07E0:0x0000 a 0x9000:0x0000
	mov	ax, 0x9000
	mov	es, ax			; ES = 0x9000
	xor	di, di			; DI = SI = 0x0
	xor	si, si
	mov	cx, kernel + 3		; tamanio del loader
					; (desde el BOF hasta el label kernel)
	shr	cx, 2			; como se hace un movsd (DOUBLE)
					; se divide el tamanio del loader
					; por cuatro -> 1 double = 4 bytes
	cld				; se pone a cero el bit D
	rep	movsd			; se realiza la copia!

; se setea un stack temporal en 0x7000:0xFFFF
	mov	ax, 0x7000
	mov	ss, ax
	mov	sp, 0xFFFF

	; hasta aca tenemos el codigo en 0x9000:0x0000
	; y una pila seteada, entonces al hacer un jmp se va a setear
	; el segmento CS con dicha direccion (comenzando a ejecutar
	; el codigo recien copiado ahi, a partir de la etiqueta segunda_etapa)
	jmp	0x9000:segunda_etapa
segunda_etapa

; se copia a partir del label kernel (donde comienza el codigo C de main.c)
; al comienzo de la memoria (direccion 0)
; OJO! DS todavia apunta al segmento viejo!
	mov	esi, kernel			; origen = ESI = kernel
	xor	edi, edi			; destino = EDI = 0x0
	mov	es, di				;se va a copiar a 0x0:0x0

	; igual que antes, se divide por cuatro ya que se copia de a un double
	mov	ecx, (KERNEL_SIZE + BSS_SIZE + 3) / 4

	; a32 realiza operaciones de 32 bits (todavia estamos en modo real!)
	; entonces el movimiento que realizamos puede ser de mas
	; de 64 Kb (en 16 bits, como maximo CX = 0xFFFF = 65535)
	a32	rep movsd
	
	; ahora que ya efectuamos el movimiento, podemos setear DS con el
	; valor correcto (0x9000) de donde esta el loader
	push	cs
	pop	ds

        ; se va a setear la GDT!

	; primero se limpian los 64Kb que va a ocupar (llenando con ceros)
	mov	eax, GDTBASE		; obtengo la direccion de la GDT
					; OJO! es la direccion fisica
	shr	eax, 4			; para accederla desde modo real
					; divido por 0x10 = 16
					; ya que al acceder al segmento el
					; hardware lo multiplica por ese
					; valor, transformando una direccion
					; lineal en fisica o real:
					; DIR LINEAL = DIR REAL / 16
	mov	es, ax			; ES = direccion lineal de la GDT
	xor	di, di			; La copia es desde el inicio
	xor	eax, eax		; Se llena con ceros
	mov	cx, 65536 / 4		; Se mueven de a 4 bytes = 1 double por
					; vez
	rep	stosd			; Se procede!

	; ahora se mueve la GDT declarada abajo hasta la ubicacion "posta"
	xor	di, di			; desde la posicion 0 de la GDT
					; (ES todavia esta seteado con la
					; direccion lineal de la GDT!)
	mov	si, inicio_gdt		; source = SI = inicio de la GDT que
					; se declaro abajo

	; tamanio de la GDT (expresado en doubles = 4 bytes)
	mov	cx, (fin_gdt - inicio_gdt + 3) / 4
	rep	movsd			; se procede a moverla!

	; y por fin la cargamos en memoria!
	lgdt	[GDT]

	; ahora que tenemos una GDT posta pasamos a PM
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; para setear el CS con el selector que queremos debemos hacer
	; un far jmp indicando dicho selector
	; como el selector que usamos para codigo tiene su base en 0x0
	; y previamente movimos el codigo del loader a 0x90000
	; entonces debemos sumarle ese valor a la etiqueta a la cual
	; vamos a efectuar el salto.  Asimismo, el valor que se ubica
	; antes de los ':' es el desplazamiento dentro de la
	; tabla de descriptores de segmento (GDT) expresado en bytes
	jmp 	dword selector_codigo:(modo_protegido + 0x90000)

; ya estamos en 32 bits!!!!!
BITS 32		
modo_protegido	
	; se carga una LDT nula
	xor	ax, ax	
	lldt	ax

	mov	ax, selector_datos
	mov	ds, ax			; DS = selector de datos
	mov	es, ax			; ES = DS
	mov	fs, ax			; FS = DS
	mov	gs, ax			; GS = DS

	mov	ax, selector_pila	; se setea el segmento de pila
	mov	ss, ax

	; en la variable stack_pointer habiamos seteado a donde debia
	; apuntar el ESP, que es al final de la memoria base
	; (nuevamente, el segmento de codigo comienza en 0x0 por lo que
	; debemos usar el offset 0x90000 para referirnos a las variables
	; declaradas en el loader)
	mov	esp, [stack_pointer + 0x90000]

	; bit reservado por intel (siempre 1)
	push	dword 0x2
	popfd

	; se hace un far jmp al codigo del kernel (main!)
	jmp	dword selector_codigo:MAIN

; /********************************************************************** 
;  * SUBRUTINAS								*
;  **********************************************************************/ 

BITS 16		; las rutinas utilizadas aca se llaman desde modo real!

; /********************************************************************** 
;  * Delay: utilizada para habilitar la compuerta A20
;  * (gracias Linus!)
;  **********************************************************************/
Delay 
	jmp .next
.next
ret

; /********************************************************************** 
;  * Empty8024: utilizada para habilitar la compuerta A20
;  * (gracias Linus!)
;  * Lo que hace es leer el teclado hasta recibir una senial de
;  * reconocimiento por parte del 8042, esto es para que el mismmo no
;  * quede en un estado "intestable"  esperando algun comando o datos.
;  * Muchas gracias Linus!!!!!!!!!!! (sacado de la version 0.0.1 de linux)
;  **********************************************************************/
Empty8042
       	call    Delay
       	in      al, 64h
       	test    al, 1
       	jz      .NoOutput
       	call    Delay
       	in      al, 60h
       	jmp     Empty8042
  .NoOutput
       	test    al, 2
       	jnz     Empty8042
ret


; /*************************************************************************** 
;  * imprimir:
;  *    imprime la cadena que se le pasa en SI hasta encontrar un '0'
;  ***************************************************************************/
imprimir:
	lodsb 
	or	al, al
	jz	.fin
	mov	ah, 0x0E
	mov	bh, 0x00
	mov	bl, 0x07
	int	0x10
	jmp	imprimir
.fin
	ret

; /********************************************************************** 
;  * FINAL SUBRUTINAS							*
;  **********************************************************************/ 

; definicion de la GDT
inicio_gdt:	
descriptor_nulo	
	dd 0x0, 0x0

descriptor_codigo		
	dw 0xFFFF	; limite (bits 0-15)
	dw 0x0		; base (bits 0-15)
	db 0x0		; base (bits 16-23)
	db 10011110b	; present, dpl=00, 1, code=1, cnf.=0, r=1, ac=0
	db 11001111b	; gran=1, use32=1, 0, 0, limit 16 - 19
	db 0x0		; base (bits 24-31)

descriptor_datos	; tambien usado para la pila!	
	dw 0xFFFF	; limit (bits 0-15)
	dw 0x0		; base (bits 0-15)
	db 0x0		; base (bits 16-23)
	db 10010010b	; present, dpl=00, 10, exp.d.=0, wrt=1, ac=0
	db 11001111b	; gran=1, big=1, 0, 0, limit 16 - 19
	db 0x0		; base (bits 24-31)			

fin_gdt:	


; /********************************************************************** 
;  * declaracion de variables usadas por sodero.asm                     *
;  **********************************************************************/ 
loader_bienvenida	db	10, 13, "Iniciando Loader de SODERO...",10,13,0
	
GDT
	GDTLimit	dw 	0xFFFF 	; GDT limit = maximum	
	GDTBase		dd 	GDTBASE	; GDT base	

IDT
	idt_limite	dw	0xFFFF
	idt_base	dd	IDTBASE

stack_pointer		dd 	0

; /********************************************************************** 
;  * fin declaracion de variables usadas por sodero.asm                 *
;  **********************************************************************/ 

bits 32

kernel:			; aca comienza lo mejor!!! bye bye assembler!!! ;-)
