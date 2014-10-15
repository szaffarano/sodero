global llamar_sys_call

;/**
; * llamar_sys_call: Funcion que se encarga de hacer de interfaz para realizar
; * el llamado a una llamada del sistema que se le pida.
; *
; * @param dword Numero que representa a la system call a llamar
; *
; * @param dword primer parametro de la system call
; * 
; * @param dword seguindo parametro de la system call
; * 
; * @param dword tercer parametro de la system call 
; *
; * @return estado_fin depende de cada system call, por lo general devuelven
; *                    estado de finalizacion de la system call llamada.
; */
ALIGN 4
llamar_sys_call:
	push	ebp            ;resguardo el valor de ebp
	mov	ebp, esp       ;reemplado ebp por esp

	push	ebx
	push	ecx
	push	edx
	
       ;paso los cuatro argumentos recibidos como parametros, utilizando los 
       ;registros de proposito general
	mov	eax, dword [ebp + 8]   ;subindice dentro del array de sys_calls
	mov	ebx, dword [ebp + 12]  ;primer argumento
	mov	ecx, dword [ebp + 16]  ;segundo argumento
	mov	edx, dword [ebp + 20]  ;tercer argumento

	int	0x40   ;llamamos a la int40, cuyo handler (sys_call_handler, 
	               ;funciones.asm) llamara al handler de la sys call 
	               ;seleccionada en eax

	pop	edx
	pop	ecx
	pop	ebx

	mov	esp, ebp        ;restauro esp
	pop	ebp             ;recupero ebp
	ret
