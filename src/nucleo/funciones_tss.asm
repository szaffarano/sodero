global cargar_tr
global ir_a_tarea

extern imprimir

segment data
%ifdef DEBUG
msg	dw	"cargar_tr: selector que cargue en el TR=0x%xw", 10, 13, 0
msg2	dw	"Selector:0x%xw - offset:0x%x", 10, 13, 0
%endif

;/**
; * Funcion que cargar el TR (Registro de Tarea de la CPU) Se utiliza una
; * sola vez por el sistema operativo para cargar la Tarea de sistema.
; * Luego este registo cambia automaticamente en el Context Switch al saltar
; * a otra tarea.
; *
; * @param selector de la tarea a cargar (Entrada en la GDT + 0 -> GDT y DPL)
; *                corresponde a la tarea de SISTEMA (init). 
; */
ALIGN 4
cargar_tr:
	push	ebp		; almaceno el valor actual de ebp
	mov	ebp, esp	; copio esp en el ebp

	xor	eax, eax
        
        mov     ax, [ebp+8] 
	ltr     ax			; Cargamos el TR	

	%ifdef DEBUG
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

;/**
; * Funcion que realiza un jmp far a la tarea a ejecutar (salto largo). 
; *
; * @param selector de la tarea a cargar (Entrada en la GDT + 0 -> GDT y DPL)
; *                corresponde a la tarea de SISTEMA (init). 
; * @param offset Desplazamiento desde el EIP de la TSS que es apuntada por el
; *              descriptor de tss dentro de la gdt que correponde a esta tarea
; *              corresponde a la tarea de SISTEMA (init). 
; */
; parametro1: selector (2 bytes)
; parametro2: offset (4 bytes)
ALIGN 4
ir_a_tarea:
        push    ebp
        mov     ebp, esp

        %ifdef DEBUG
        mov	eax, [ebp + 12]
        push	eax

        xor	eax, eax
        mov	eax, [ebp + 8]
        push	eax

        push	dword msg2

        call imprimir
        %endif

        sub     esp, 6
        mov     eax, [ebp + 8]
        mov     word [ebp - 2],ax
        mov     eax, [ebp + 12]
        mov     dword [ebp - 6],eax

        jmp     dword far [ebp - 6]
        ;call     dword [ebp - 6]

        mov     esp, ebp
        pop     ebp
ret
