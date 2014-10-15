global get_ss
global get_es
global get_cs
global get_ds
global get_fs
global get_gs
global get_esp

;/**
; * get_ss: Funcion que se encarga de obtener el descriptor del segmento de
; * de pila (ss) y devolverlo en eax.
; *
; * @return ss valor del descriptor del segmento de pila
; */
ALIGN 4
get_ss:
	xor	eax, eax
	mov	ax, ss
	ret

;/**
; * get_es: Funcion que se encarga de obtener el descriptor del segmento 
; * auxiliar es y devolverlo en eax.
; *
; * @return es valor del descriptor del segmento de pila
; */
ALIGN 4
get_es:
	xor	eax, eax
	mov	ax, es
	ret

;/**
; * get_cs: Funcion que se encarga de obtener el descriptor del segmento 
; * codigo cs y devolverlo en eax.
; *
; * @return cs valor del descriptor del segmento de codigo
; */
ALIGN 4
get_cs:
	xor	eax, eax
	mov	ax, cs
	ret

;/**
; * get_ds: Funcion que se encarga de obtener el descriptor del segmento 
; * de datos (ds) y devolverlo en eax.
; *
; * @return ds valor del descriptor del segmento de datos
; */
ALIGN 4
get_ds:
	xor	eax, eax
	mov	ax, ds
	ret

;/**
; * get_fs: Funcion que se encarga de obtener el descriptor del segmento 
; * auxiliar fs y devolverlo en eax.
; *
; * @return fs valor del descriptor del segmento de pila
; */
ALIGN 4
get_fs:
	xor	eax, eax
	mov	ax, fs
	ret

;/**
; * get_gs: Funcion que se encarga de obtener el descriptor del segmento 
; * auxiliar gs y devolverlo en eax.
; *
; * @return gs valor del descriptor del segmento de pila
; */
ALIGN 4
get_gs:
	xor	eax, eax
	mov	ax, gs
	ret

;/**
; * get_esp: Funcion que se encarga de obtener el valor del stack pointer.
; *
; * @return esp valor del stack pointer. 
; */
ALIGN 4
get_esp:
	xor	eax, eax
	mov	eax, esp
	ret

