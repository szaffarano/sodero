STACK_POINTER		equ     0x7C00
CARGA_FAT		equ     0x0500
COMIENZO_LOADER	equ     0x07E0

[bits 16]
org 0x7C00

	jmp short comienzo	;salto al inicio del bootstrap propiamente dicho
	nop
	; datos en la BPB para compatibilidad con FAT (12)
	oemId				db	"SODERO  "
	bytesPS				dw	0x0200			; cada sector es de 512
											; bytes
	sectoresPorCluster	db	0x01			; un sector por cluster
	sectoresReservados	dw	0x0001			; un sector para el boot
											; strap
	totalFATs			db	0x02			; la posta y un backup
	entradasRaiz		dw	0x00E0			; 224 entradas en la 
											; tabla raiz
	totalSectores		dw	0x0B40 			; si la cant. de 
											; sectores es menor 
											; a 65535 van aca, sino
   											; va cero
	descriptor			db	0xF0			; descriptor del 
											; dispositivo(?)
	sectoresPorFat		dw	0x09			; sectores por fat
	sectoresPorPista	dw	0x12			; sectores por pista
	cantHeads			dw	0x02			; numero de cabezas
	sectoresOcultos		dd	0x00000000		; numero de sectores 
											; ocultos
	cantSectores		dd	0x00000000		; cuando los sectores 
											; son mayores a 65535 
											; entonces 
											; totalSectores = 0 
											; y aca va dicho valor
	nroDisp				db	0x00			; numero de dispositivo 
											; fisico
	flags				db	0x00			; no se usa, reservado
	signature			db	0x29			; firma...
	idVolumen			dd	0x0FFFFFFF		; aca va fruta
	nombreVolumen		db	"SODERO BOOT"
	tipoFS				db	"FAT12   "

comienzo:
	cli						; deshabilitar interrupciones
	xor	ax, ax
	mov	ss, ax				; seteo el SS

	mov	sp, STACK_POINTER	; seteo el SP

	sti						; habilitar interrupciones

	push	cs
	pop		ds				; seteo DS

	push	cs
	pop		es				; seteo ES

	push	cs
	pop		fs				; seteo FS

	push	cs
	pop		gs				; seteo GS


   	; borrar la pantalla (se realiza al setear el modo de video)
   	; en particular el modo 0x03 es texto, 25 filas X 80 columnas
	mov		ax, 0x3
	int		0x10

	mov	si, msg_carga	; imprimir mensaje
	call print

cargar:
	; CX = tamanio del directorio raiz (donde se busca el loader)
	; CX = ( entradasRaiz * 32 bytes ) / bytesPorSector
	; Nota: cada entrada tiene 32 bytes
	xor	ax, ax
	xor	dx, dx
	mov	ax, 32
	mul	word [entradasRaiz]
	div	word [bytesPS]
	xchg	ax, cx

	; AX = direccion del directorio raiz
	; AX = totalFATs * sectoresPorFat + sectoresReservados
	mov	al, byte [totalFATs]
	mul	byte [sectoresPorFat]
	add	ax, word [sectoresReservados]

	; sect_dato = base del directorio raiz + tamanio del directorio raiz
	; sect_dato = sector inicial de los datos
	mov	word [sect_dato], ax
	add	word [sect_dato], cx

	; leer el directorio raiz en ES:CARGA_FAT = 0x0000:CARGA_FAT
	mov	bx, CARGA_FAT
	call	leer_sectores

	; buscar dentro del directorio raiz el nombre del kernel
	mov	cx, word [entradasRaiz]		; hay [entradasRaiz] entradas 
						; en el directorio
	mov	di, CARGA_FAT			; se leyo en ES:CARGA_FAT
l1:
	push	cx
	mov	cx, 0x000B			; cada entrada tiene 11 
						; caracteres
	mov	si, nombre_kernel		; nombre a comparar con 
						; c/entrada
	push	di				; guardo la posicion inicial
	rep	cmpsb				; comparo SI con DI de a un byte

	pop	di				; recupero DI
	je	cargar_fat			; si encontro el archivo lo 
						; cargo
	pop	cx				; si no lo encontro, recupero 
						; CX e incremento DI para que 
	add	di, 0x0020			; apunte a la siguiente entrada
						; en la tabla
	loop	l1

	jmp	error_carga			; si llegue hasta aca es 

error_carga1:
	jmp	error_carga

; antes de cargar la FAT necesitamos guardar la posicion del archivo
cargar_fat:
	mov	dx, word [di + 0x001A]		; en DI teniamos el inicio de 
						; la entrada en la tabla de 
						; directorios de [nombre_kernel]
						; y dentro de dicha entrada
						; a un offset de 0x001A esta el
						; cluster inicial del mismo
	mov	word [cluster], dx		; que es almacenado en la 
						; variable auxiliar [cluster]

	; ahora si podemos proceder a cargar la FAT en memoria
	; CX = cantidad total de sectores de la FAT (cant de sectores a leer)
	xor	ax, ax
	mov	al, byte [totalFATs]
	mul	word [sectoresPorFat]
	mov	cx, ax

	; se calcula la ubicacion de la FAT y se guarda en AX
	mov	ax, word [sectoresReservados]

	; se lee la FAT en ES:CARGA_FAT
	mov	bx, CARGA_FAT
	call	leer_sectores


	; en este punto ya tenemos la FAT en memoria
	; se lee la imagen en COMIENZO_LOADER:0x0000
	mov     ax, COMIENZO_LOADER
	push    ax
	pop     es
	xor     bx, bx
	push    bx

cargar_kernel:
	mov	ax, word [cluster]		; cluster a leer
	pop	bx				; buffer de lectura para el 
						; cluster

	call	cluster2lba			; convertir la direccion en LBA

	xor	cx, cx
	mov	cl, byte [sectoresPorCluster]	; cantidad de sectores a leer
	call leer_sectores

	push bx
	; calcular el proximo cluster = 3/2 * cluster_actual
	mov	bx, word [cluster]
	mov	ax, word [cluster]

	shl	bx, 1
	add	bx, ax
	shr	bx, 1
	and	ax, 1
	mov	ax, [CARGA_FAT + BX]
	jz	cluster_par
cluster_impar:
	shr	ax, 4
cluster_par:
	and	ax, 0000111111111111b		; obtengo los ultimos 12 digitos

	mov	word [cluster], ax		; si el cluster nuevo es 0x0FF0
	cmp	ax, 0x0FF0			; llegue al fin de archivo

	jb	cargar_kernel

	mov	ax,COMIENZO_LOADER	; Set segment registers
	push	ax
	pop	ds
	push	ax
	pop	es
	push	ax
	pop	fs
	push	ax
	pop	gs

	; se escribe al port de la diskettera para parar el motor
	; y que no siga girando al pedo
	xor	ax, ax		; al = 0 -> comando para parar el motor
	mov	dx, 0x3F2	; port 0x3F2 -> diskettera
	out	dx, al		; la escritura propiamente dicha

	jmp 	COMIENZO_LOADER:0x0000

error_carga:
	mov	si, msg_error
	call	print
	xor	ah, ah
	int	0x16
	int	0x19

;/*************************************************************************** 
; * -----------------------------Subrutinas----------------------------------
; ***************************************************************************/

;/*************************************************************************** 
; * leer_sectores
; *    lee [CX] sectores a partir de [AX] y los almacena en la ubicacion de
; *    memoria apuntada por ES:BX
; ***************************************************************************/
leer_sectores:
main:
	mov	di, 0x0005		; 5 reintentos antes de dar error
sector_loop:
	push	ax
	push	bx
	push	cx
	call lba2chs

	mov	ah, 0x02		; voy a leer
	mov	al, 0x01		; cant de sectores = 1
	mov	ch, byte [pista]
	mov	cl, byte [sector]
	mov	dh, byte [cabeza]
	mov	dl, byte [nroDisp]
	int	0x13			; leer!
	jnc	lectura_ok		; si no hubo carry --> lectura exitosa

	xor	ax, ax
	int	0x13			; si hubo error se resetea la disketera
	dec	di			; se decrementa el contador de intentos
	pop	cx
	pop	bx
	pop	ax
	jnz	sector_loop		; y si no me pase, reintento la lectura

	int 0x18			; sino bootea --> mejorar manejo de 
					; errores!!

lectura_ok:
	mov	si, punto		; imprimir un punto cada vez que se lee
	call print
	pop	cx
	pop	bx
	pop	ax
	add	bx, word [bytesPS]	; incrementar el buffer de lectura
	inc	ax			; apuntar al siguiente sector
	loop	main			; leer el siguiente sector

	ret				; no hay mas sectores para leer (CX=0)

;/*************************************************************************** 
; * cluster2LBA
; *    convierte un cluster FAT en una direccion LBA (Linear Block Addresing)
; *    ( por ejemplo, LBA #1 = cyl: 0 / head: 0 / sector: 1 )
; *    LBA = ( cluster - 2 ) * sectoresPorCluster
; *
; *    LBA = ( cluster - 2 ) * sectoresPorCluster
; * IN:  AX = numero de cluster
; * OUT: AX = LBA #
; ***************************************************************************/
cluster2lba:
	dec	ax				; AX = AX - 2
	dec	ax

	xor	cx, cx
	mov	cl, byte [sectoresPorCluster]	; AX = AX * sectoresPorCluster
	mul	cx
	add	ax, word [sect_dato]		; AX = AX + sect_dato

	ret

;/*************************************************************************** 
; * lba2chs
; *    convierte una direccion LBA en CHS ( Cyl/Head/Sect)
; * donde:
; *    sector = (sectorLogico / sectoresPorPista) + 1
; *    cabeza = (sectorLogico / sectoresPorPista) MOD cantHeads
; *    pista = sectorLogico / (sectoresPorPista * cantCabezas)
; ***************************************************************************/
lba2chs:
	xor	dx, dx
	div	word [sectoresPorPista]
	inc	dl
	mov	byte [sector], dl

	xor	dx, dx
	div	word [cantHeads]
	mov	byte [cabeza], dl
	mov	byte [pista], al

	ret

;/*************************************************************************** 
; * print:
; *    imprime la cadena que se le pasa en SI hasta encontrar un '0'
; ***************************************************************************/
print:
	lodsb 
	or	al, al
	jz	.end
	mov	ah, 0x0E
	mov	bh, 0x00
	mov	bl, 0x07
	int	0x10
	jmp	print
.end:
	ret

;/*************************************************************************** 
; * -----------------------------Variables-----------------------------------
; ***************************************************************************/
	; auxiliares que me indican lo que se va leyendo
	sector		db	0x00
	cabeza		db	0x00
	pista		db	0x00

	; base (en cant de sectores) de donde comienzan los datos
	sect_dato	dw	0x0000

	; cluster que se va a leer
	cluster		dw	0x0000

	; nombre del kernel (formato 8+3)
	nombre_kernel	db	"SODERO  SYS"

	msg_carga	db	"Cargando SODERO", 0
	punto		db	".", 0
	msg_error	db	 10, 13, "Error Carga", 0

 ; se rellena con ceros hasta 2 bytes antes de terminar el sector
 times 510-($-$$) db 0
 ; los ultimos dos bytes del sector se setean con los valores 0xAA55
 dw 0xAA55
