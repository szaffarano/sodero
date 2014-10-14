; ********************************************************************
; *                                                                  *
; *  765NODMA.ASM 2.0   -   Programa de demostración de acceso a     *
; *                         bajo nivel al disquete sin emplear DMA.  *
; *                                                                  *
; ********************************************************************

; ************ Macros de propósito general.

XPUSH          MACRO regmem            ; apilar lista de registros
                 IRP rm, <regmem>
                   PUSH rm
                 ENDM
               ENDM

XPOP           MACRO regmem            ; desapilar lista de registros
                 IRP rm, <regmem>
                   POP rm
                 ENDM
               ENDM

; ************ Programa principal.

fdc_test       SEGMENT
               ASSUME CS:fdc_test, DS:fdc_test

               ORG   100h

main           PROC
               CALL  menu              ; opciones
               DEC   AL
               JZ    leer              ; opción de leer sector
               DEC   AL
               JZ    escribir          ; opción de escribirlo
               LEA   DX,adios_txt
               CALL  print             ; opción de salir:
               MOV   AX,40h
               MOV   DS,AX
               MOV   AL,DS:[8Bh]       ; velocidad previa al programa
               MOV   CL,6
               SHR   AL,CL             ; pasarla a bits 0..1
               MOV   DX,3F7h
               OUT   DX,AL             ; restaurar velocidad previa
               INT   20h
leer:          LEA   DX,cls_txt
               CALL  print             ; borrar pantalla
               LEA   DX,lectura_txt
               CALL  print             ; mensaje inicial
               LEA   DX,aviso_txt
               CALL  print
               CALL  pide_sector       ; pedir pista, cabeza, ...
               MOV   orden,F_READ
               CALL  init_drv
               CALL  recalibrar
               JC    fallo
               CALL  seek_drv
               JC    fallo
               LEA   DI,buffer
               CALL  sector_io         ; cargar dicho sector
               JC    fallo
               CALL  imprime_sector    ; mostrar su contenido
               JMP   main
escribir:      LEA   DX,cls_txt
               CALL  print             ; limpiar pantalla
               LEA   DX,escritura_txt
               CALL  print             ; mensaje inicial
               LEA   DX,aviso_txt
               CALL  print
               CALL  pide_sector       ; pedir pista, cabeza, ...
               CALL  pide_relleno      ; pedir byte de relleno
               MOV   orden,F_WRITE
               CALL  init_drv
               CALL  recalibrar
               JC    fallo
               CALL  seek_drv
               JC    fallo
               LEA   DI,buffer
               CALL  sector_io         ; grabar dicho sector
               JC    fallo
               JMP   main
fallo:         LEA   DX,fallo_txt      ; mensaje de error
               CALL  print
               CALL  getch
               JMP   main
main           ENDP

; ************ Subrutinas de apoyo

menu           PROC
               LEA   DX,cls_txt
               CALL  print
               LEA   DX,opciones_txt   ; texto del menú
               CALL  print
espera_opc:    CALL  getch
               CMP   AL,'1'
               JE    opc_ok            ; elegida opción 1
               CMP   AL,'2'
               JE    opc_ok            ; elegida opción 2
               CMP   AL,27
               JE    opc3_ok           ; ESC (opción '3')
               CMP   AX,2D00h
               JNE   espera_opc        ; no es ALT-X
opc3_ok:       MOV   AL,'3'
opc_ok:        SUB   AL,'0'
               RET
menu           ENDP

; ------------ Solicitar información del sector a ser accedido.

pide_sector    PROC
               LEA   DX,unidad_txt
               CALL  input_AL          ; pedir unidad
               MOV   unidad,AL
               LEA   DX,vunidad_txt
               CALL  input_AL          ; seleccionar velocidad
               MOV   vunidad,AL
               LEA   DX,tdisco_txt
               CALL  input_AL          ; problema de 40/80 pistas
               MOV   tunidad,AL
               LEA   DX,tamano_txt
               CALL  input_AL          ; preguntar tamaño sector
               MOV   tsector,AL
               LEA   DX,gap_rw_txt
               CALL  input_AL          ; preguntar tamaño sector
               MOV   gap,AL
               LEA   DX,pista_txt
               CALL  input_AL          ; pedir pista
               MOV   cilindro,AL
               LEA   DX,cabeza_txt
               CALL  input_AL          ; pedir cabeza
               MOV   cabezal,AL
               LEA   DX,sector_txt
               CALL  input_AL          ; pedir sector
               MOV   sector_ini,AL
               MOV   sector_fin,AL
               MOV   CL,tsector
               MOV   CH,0
               INC   CX                ; CX: 1-128 bytes, 2-256, ...
               MOV   AX,64
computab:      SHL   AX,1
               LOOP  computab
               MOV   bsector,AX        ; bytes/sector
               MOV   AL,cabezal
               SHL   AL,1
               SHL   AL,1
               OR    AL,unidad
               MOV   byte1,AL          ; byte 1 común a muchas órdenes
               RET
pide_sector    ENDP

; ------------ Imprimir sector en hex/ASCII en bloques de 256 bytes.

imprime_sector PROC
               LEA   BX,buffer
               MOV   AX,bsector
               MOV   CL,AH
               MOV   CH,0              ; CX secciones de 256 bytes
               AND   CX,CX
               JNZ   otra_mitad
               INC   CX                ; al menos imprimir una vez
otra_mitad:    PUSH  CX
               LEA   DX,cls_txt
               CALL  print
               MOV   CX,16             ; 16 líneas
otra_linea:    PUSH  CX
               MOV   CX,16             ; de 16 caracteres
pr_hexa:       MOV   AL,' '
               CALL  printAL
               MOV   AL,[BX]
               INC   BX
               CALL  print8hex
               LOOP  pr_hexa
               MOV   AL,' '
               CALL  printAL
               CALL  printAL
               SUB   BX,16
               MOV   CX,16
pr_ascii:      MOV   AL,[BX]
               INC   BX
               CMP   AL,' '
               JAE   ascii_ok
               MOV   AL,'.'
ascii_ok:      CALL  printAL
               LOOP  pr_ascii
               MOV   AL,13
               CALL  printAL
               MOV   AL,10
               CALL  printAL
               POP   CX
               LOOP  otra_linea
               LEA   DX,ptecla_txt
               CALL  print
               CALL  getch
               POP   CX
               LOOP  otra_mitad
               RET
imprime_sector ENDP

; ------------ Pedir byte para llenar el sector a grabar.

pide_relleno   PROC
               LEA   DX,relleno_txt
               CALL  input_AL
               LEA   DI,buffer
               MOV   CX,bsector        ; tamaño de sector en bytes
               CLD
               REP   STOSB
               RET
pide_relleno   ENDP

; ------------ Imprimir cadena en DS:DX terminada en un '$'.

print          PROC
               PUSH  AX
               MOV   AH,9              ; función de impresión
               INT   21h               ; llamar al sistema
               POP   AX
               RET
print          ENDP

; ------------ Imprimir carácter en AL

printAL        PROC
               PUSH  AX
               PUSH  DX                ; registros usados preservados
               MOV   AH,2              ; función de impresión del DOS
               MOV   DL,AL             ; carácter a imprimir
               INT   21h               ; llamar al sistema
               POP   DX
               POP   AX                ; recuperar registros
               RET                     ; retornar
printAL        ENDP

; ------------ Imprimir carácter hexadecimal (AL).

print4hex      PROC
               PUSH  AX                ; preservar AX
               ADD   AL,'0'            ; pasar binario a ASCII
               CMP   AL,'9'
               JBE   no_sup9           ; no es letra
               ADD   AL,'A'-'9'-1      ; lo es
no_sup9:       CALL  printAL           ; imprimir dígito hexadecimal
               POP   AX                ; restaurar AX
               RET
print4hex      ENDP

; ------------ Imprimir byte hexadecimal en AL.

print8hex      PROC
               PUSH  CX
               PUSH  AX
               MOV   CL,4
               SHR   AL,CL             ; pasar bits 4..7 a 0..3
               CALL  print4hex         ; imprimir nibble más
significativo
               POP   AX                ; restaurar AL
               PUSH  AX                ; y preservarlo de nuevo
               AND   AL,1111b          ; dejar nibble menos significativo
               CALL  print4hex         ; e imprimirlo
               POP   AX
               POP   CX
               RET
print8hex      ENDP

; ------------ Esperar pulsación de tecla y devolverla en AX.

getch          PROC
               MOV   AH,1              ; esperar carácter (algunos
               INT   16h               ; KEYB de XT se cuelgan
               JZ    getch             ; al usar directamente el
               MOV   AH,0              ; servicio 0).
               INT   16h
               RET
getch          ENDP

; ------------ Leer nº decimal de hasta 3 dígitos y devolverlo en AL.

input_AL       PROC
               PUSH  BX
               PUSH  CX
               PUSH  DX
               PUSH  AX
pedir_dato:    CALL  print
               MOV   AH,0Ah            ; función de entrada (teclado)
               LEA   DX,buffer
               MOV   BX,DX
               MOV   WORD PTR [BX],4   ; (inicializar dos variables)
               INT   21h               ; llamar al sistema
               MOV   CL,[BX+1]
               XOR   CH,CH             ; número de caracteres pulsados
               POP   AX
               POP   DX
               PUSH  DX
               PUSH  AX
               JCXZ  pedir_dato        ; se pulsó RETURN: reiterar
               XOR   DX,DX
gen_num:       MOV   AX,10
               MUL   DX
               MOV   DX,AX
               MOV   AL,[BX+2]
               SUB   AL,'0'
               INC   BX
               XOR   AH,AH
               ADD   DX,AX
               LOOP  gen_num           ; conversión ASCII -> binario
               POP   AX
               MOV   AL,DL             ; resultado
               POP   DX
               POP   CX
               POP   BX
               RET
input_AL       ENDP

; ------------ Encender motor y esperar a que tome cierta velocidad.

init_drv       PROC
               PUSH  CX
               CALL  reset_drv
               MOV   CX,18
               CALL  retardo           ; esperar aceleración disco
               POP   CX
               RET
init_drv       ENDP

; ------------ Establecer modalidad de operación del controlador
;              y asegurar que el motor está en marcha.

reset_drv      PROC
               XPUSH <DS, AX, BX, CX, DX>
               PUSH  DS
               MOV   BX,40h                 ; engañar al BIOS para
               MOV   DS,BX                  ; que no pare el motor al
               MOV   BYTE PTR DS:[BX],255   ; menos durante 14 seg.
               POP   DS
               MOV   DX,3F2h           ; registro de salida digital
               MOV   CL,unidad
               ADD   CL,4
               MOV   AL,1
               SHL   AL,CL             ; colocar bit del motor
               OR    AL,unidad         ; seleccionar unidad; NO DMA
               OUT   DX,AL             ; reset
               OR    AL,00000100b
               JMP   SHORT $+2
               OUT   DX,AL             ; fin del reset
               CALL  espera_int
               MOV   AL,3
               CALL  fdc_write         ; Comando 'Specify':
               MOV   AL,0DFh
               CALL  fdc_write
               MOV   AL,3              ; modo NO DMA
               CALL  fdc_write         ; head load y modo
               PUSH  DS
               MOV   BX,40h
               MOV   DS,BX
               MOV   CL,CS:unidad
               MOV   AL,1
               SHL   AL,CL
               AND   BYTE PTR DS:[BX-1],11110000b
               OR    DS:[BX-1],AL      ; indicar motor ON
               POP   DS
               MOV   DX,3F7h
               MOV   AL,vunidad        ; velocidad de transferencia
               OUT   DX,AL
               XPOP  <DX, CX, BX, AX, DS>
               RET
reset_drv      ENDP

; ------------ Recalibrar la unidad (si hay error se intenta otra vez
;              para el caso de que deba moverse más de 77 pistas).

recalibrar     PROC
               XPUSH <AX, CX>
               MOV   CX,2              ; dos veces como mucho
recalibra:     CALL  habilita_int
               MOV   AL,7
               CALL  fdc_write         ; comando de 'recalibrado'
               JZ    fallo_recal
               MOV   AL,byte1
               CALL  fdc_write         ; enviar HD, US1, US0
               JZ    fallo_recal
               CALL  espera_int        ; esperar interrupción
               JZ    fallo_recal
               MOV   AL,8
               CALL  fdc_write         ; comando 'leer estado int...'
               JZ    fallo_recal
               CALL  fdc_read          ; leer registro de estado 0
               JZ    fallo_recal
               MOV   AH,AL
               CALL  fdc_read          ; leer cilindro actual
               XOR   AH,00100000b      ; bajar bit de 'seek end'
               TEST  AH,11110000b      ; comprobar resultado y ST0
               JNZ   fallo_recal       ; sin 'seek end' o sin TRK0
               XPOP  <CX, AX>
               CLC                     ; Ok.
               RET
fallo_recal:   LOOP  recalibra         ; reintentar comando
               XPOP  <CX, AX>
               STC                     ; condición de fallo
               RET
recalibrar     ENDP

; ------------ Llevar el cabezal a la pista indicada.

seek_drv       PROC
               XPUSH <AX, CX>
               CLI
               CALL  habilita_int      ; usar interrupciones
               MOV   AL,0Fh
               CALL  fdc_write         ; comando 'seek'
               JZ    fallo_seek
               MOV   AL,byte1
               CALL  fdc_write         ; enviar HD, US1, US0
               MOV   AL,cilindro
               CMP   tunidad,0
               JE    pista_ok          ; es unidad de doble densidad
               CMP   vunidad,1         ; es de alta:
               JNE   pista_ok          ; no es disco 5¼-360
               SHL   AL,1              ; cilindro=cilindro*2
pista_ok:      CALL  fdc_write         ; enviar cilindro
               CALL  espera_int        ; esperar interrupción
               CLI
               MOV   AL,8
               CALL  fdc_write         ; comando 'leer estado int...'
               JZ    fallo_seek
               CALL  fdc_read          ; leer registro de estado 0
               CALL  fdc_read          ; leer cilindro actual
               STI
               MOV   CX,1
               CALL  retardo           ; esperar asentamiento cabezal
               XPOP  <CX, AX>
               CLC                     ; retornar con éxito
               RET
fallo_seek:    STI
               XPOP  <CX, AX>
               STC                     ; retornar indicando fallo
               RET
seek_drv       ENDP

; ------------ Habilitar interrupción disquete (y modo DMA).

habilita_int   PROC
               XPUSH <AX, CX, DX>
               MOV   CL,unidad
               ADD   CL,4
               MOV   AL,1
               SHL   AL,CL             ; colocar bit del motor
               OR    AL,unidad         ; seleccionar unidad
               OR    AL,00000100b      ; no hacer reset
               MOV   DX,3F2h
               OUT   DX,AL
               OR    AL,00001000b      ; modo DMA
               JMP   SHORT $+2
               OUT   DX,AL
               XPOP  <DX, CX, AX>
               RET
habilita_int   ENDP

; ------------ Esperar interrupción de disquete y volver de nuevo al
;              modo NO DMA (lo que inhibe interrupción disquete).

espera_int     PROC
               STI
               XPUSH <AX, CX>
               XPUSH <DS, 40h>
               POP   DS
               MOV   AH,0FFh
esperar_int:   CMP   AL,DS:[6Ch]
               JE    mira_int
               MOV   AL,DS:[6Ch]
               INC   AH
               CMP   AH,37             ; no esperar más de 2 segundos
               JA    fin_espera        ; timeout
mira_int:      TEST  BYTE PTR DS:[3Eh],80h
               JZ    esperar_int
fin_espera:    AND   BYTE PTR DS:[3Eh],127  ; resetear flag
               POP   DS                     ; para futura interrupción
               MOV   CL,unidad
               ADD   CL,4
               MOV   AL,1
               SHL   AL,CL             ; colocar bit del motor
               OR    AL,unidad         ; seleccionar unidad
               OR    AL,00000100b      ; no hacer reset y no DMA
               MOV   DX,3F2h
               OUT   DX,AL
               XPOP  <CX, AX>
               RET
espera_int     ENDP

; ------------ Cargar o escribir CX sector(es) del disco en ES:DI,
;              actualizando la dirección en ES:DI pero sin alterar
;              ningún otro registro. Si hay error se devuelve CF=1 y
;              no se modifica ES:DI. En el momento crítico en que se
;              leen/escriben los sectores, no se llama a las
;              subrutinas habituales por razones de velocidad, lo
;              que implica duplicar código y alargar el programa.

sector_io      PROC
               XPUSH <AX, BX, CX, DX, DI>
               MOV   AL,orden
               CLI
               CALL  fdc_write         ; comando leer/escribir del 765
               JNZ   io_proc
               JMP   sector_io_ko
io_proc:       MOV   AL,byte1
               CALL  fdc_write         ; enviar HD, US1, US0
               MOV   AL,cilindro
               CALL  fdc_write         ; enviar cilindro
               MOV   AL,cabezal
               CALL  fdc_write         ; enviar cabezal
               MOV   AL,sector_ini
               CALL  fdc_write         ; enviar nº sector
               MOV   AL,tsector
               CALL  fdc_write         ; longitud sector
               MOV   AL,sector_fin
               CALL  fdc_write         ; último sector
               MOV   AL,gap
               CALL  fdc_write         ; GAP de lectura/escritura
               MOV   AL,128
               CALL  fdc_write         ; tamaño sector si longitud=0
               CLD
               MOV   AL,sector_fin
               SUB   AL,sector_ini
               INC   AL
               XOR   AH,AH             ; AX = nº de sectores
               MUL   bsector
               MOV   CX,AX             ; bytes a leer/escribir
               MOV   DX,3F4h           ; registro de estado del FDC
espera_exec:   IN    AL,DX
               TEST  AL,80h            ; ¿alcanzada fase ejecución?
               JZ    espera_exec
               CMP   orden,F_WRITE
               JE    fdc_wr_sect
fdc_rd_sect:   IN    AL,DX
               TEST  AL,80h            ; ¿listo para E/S?
               JZ    fdc_rd_sect
               TEST  AL,16
               JZ    sector_io_ko      ; fallo en lectura
               INC   DX                ; apuntar al registro de datos
               IN    AL,DX             ; leer byte del sector
               DEC   DX
               STOSB                   ; ES:[DI++] <-- AL
               LOOP  fdc_rd_sect       ; repetir hasta fin sector(es)
               JMP   sect_io_fin
fdc_wr_sect:   IN    AL,DX
               TEST  AL,80h            ; ¿listo para E/S?
               JZ    fdc_wr_sect
               TEST  AL,64
               JNZ   sector_io_ko      ; fallo en escritura
               MOV   AL,ES:[DI]
               INC   DX                ; apuntar al registro de datos
               OUT   DX,AL             ; escribir byte del sector
               DEC   DX
               INC   DI
               LOOP  fdc_wr_sect       ; hasta acabar sector(es)
sect_io_fin:   MOV   CX,7
sect_io_rx:    CALL  fdc_read          ; leyendo resultados del éxito
               LOOP  sect_io_rx
               STI                     ; ...fin de la fase crítica
               POP   CX                ; «sacar» DI sin cambiarlo
               CLC                     ; indicar éxito
               JMP   sector_io_fin
sector_io_ko:  MOV   DX,3F4h           ; leer resultados del fallo
kill_info:     IN    AL,DX
               TEST  AL,80h            ; ¿listo para E/S?
               JZ    kill_info
               TEST  AL,64
               JZ    info_killed       ; el 765 no devuelve datos
               INC   DX                ; apuntar al registro de datos
               IN    AL,DX             ; leer byte de resultados
               DEC   DX
               JMP   kill_info
info_killed:   STI
               POP   DI                ; anular cambio de DI
               STC                     ; indicar fallo
sector_io_fin: XPOP  <DX, CX, BX, AX>
               RET
sector_io      ENDP

; ------------ Recibir byte del FDC en AL. A la vuelta, ZF = 1 si
;              la operación fracasó (el FDC no estaba listo).

fdc_read       PROC
               PUSH  CX
               PUSH  DX
               MOV   DX,3F4h           ; registro de estado del FDC
               XOR   CX,CX             ; evitar cuelgue total si falla
espera_rd:     IN    AL,DX             ; leer registro de estado
               TEST  AL,80h            ; ¿bit 7 inactivo?
               LOOPZ espera_rd         ; así es: el FDC está ocupado
               INC   DX                ; apuntar al registro de datos
               IN    AL,DX             ; leer byte del FDC
               AND   CX,CX             ; ZF = 1 si fallo al leer
               POP   DX
               POP   CX
               RET
fdc_read       ENDP

; ------------ Enviar byte AL al FDC. A la vuelta, ZF = 1 si
;              la operación fracasó (el FDC no estaba listo).

fdc_write      PROC
               PUSH  AX
               PUSH  CX
               PUSH  DX
               MOV   DX,3F4h           ; registro de estado del FDC
               XCHG  AH,AL             ; preservar AL en AH
               XOR   CX,CX             ; evitar cuelgue total si falla
espera_wr:     IN    AL,DX             ; leer registro de estado
               TEST  AL,80h            ; ¿bit 7 inactivo?
               LOOPZ espera_wr         ; así es: el FDC está ocupado
               XCHG  AH,AL             ; recuperar el dato de AL
               INC   DX                ; apuntar al registro de datos
               OUT   DX,AL             ; enviar byte al FDC
               AND   CX,CX             ; ZF = 1 si fallo al escribir
               POP   DX
               POP   CX
               POP   AX
               RET
fdc_write      ENDP

; ------------ Esperar CX 1/18,2 avos de segundo.

retardo        PROC
               PUSH  DS
               PUSH  AX
               PUSH  CX
               MOV   AX,40h
               MOV   DS,AX
               STI
espera_tics:   MOV   AX,DS:[6Ch]       ; esperar que el contador
espera_tic:    CMP   AX,DS:[6Ch]       ; de hora del BIOS...
               JE    espera_tic
               LOOP  espera_tics       ; ... cambie lo suficiente
               POP   CX
               POP   AX
               POP   DS
               RET
retardo        ENDP

; ************ Mensajes

cls_txt        DB    10,10,10,10,10,10,10,10,10,10,10,10              
DB    10,10,10,10,10,10,10,10,10,10,10,10,13,"$"

opciones_txt   DB    "765NODMA.ASM 2.0  -  Acceso a disquete sin DMA."
               DB    13,10," (c) 1991 Jesús Arias Alvarez."
               DB    13,10," (c) 1992, 1993 Ciriaco García de Celis."
               DB    13,10,10,9,"1.- Leer sector"
               DB    13,10,9,"2.- Escribir sector"
               DB    13,10,10,9,"  ESC-Salir"
               DB    13,10,10,"  Elige una opción: $"

lectura_txt    DB    13,10,"Lectura de sector.$"

escritura_txt  DB    13,10,"Escritura de sector.$"

aviso_txt      DB    13,10,"--------------------",13,10
               DB    "Aviso: No se validan las entradas.",10,"$"

adios_txt      DB    13,"  Hasta luego.                 ",13,10,"$"

ptecla_txt     DB    13,10,"- Estás viendo 256 bytes del sector."
               DB    13,10,"- Pulsa una tecla para continuar.$"

fallo_txt      DB    13,10,10,"¡Fallo al acceder al disco!",7,"$"

unidad_txt     DB    13,10,"Unidad (0-A, 1-B): $"
vunidad_txt    DB    13,10,"Velocidad: "
               DB    13,10,"  (0) 500 Kbaudios (5¼ HD y 3½ HD)"
               DB    13,10,"  (1) 300 Kbaudios (5¼ DD)"
               DB    13,10,"  (2) 250 Kbaudios (3½ DD)"
               DB    13,10,"  Elige: $"
tdisco_txt     DB    13,10,"Disquete 40 pistas en unidad de 80: "
               DB    "(1) sí, (0) no: $"
tamano_txt     DB    13,10,"Tamaño de sector (2->512 bytes): $"
gap_rw_txt     DB    13,10,"Tamaño del GAP (41-DD, 27-HD):   $"
pista_txt      DB    13,10,"Pista:   $"
cabeza_txt     DB    13,10,"Cabezal: $"
sector_txt     DB    13,10,"Sector:  $"
relleno_txt    DB    13,10,"Byte para inicializar sector:  $"

; ************ Datos

F_READ         EQU   01100110b         ; orden de lectura del FDC
F_WRITE        EQU   01000101b         ; orden de escritura del FDC

orden          DB    ?                 ; orden a procesar
unidad         DB    ?
vunidad        DB    ?                 ; velocidad de transferencia
tunidad        DB    ?                 ; control de salto de pista
cilindro       DB    ?                 ; pista del disco a usar
cabezal        DB    ?                 ; cabeza
sector_ini     DB    ?                 ; sector inicial
sector_fin     DB    ?                 ; sector final
tsector        DB    ?                 ; tamaño de sector (logaritmo)
bsector        DW    ?                 ; tamaño de sector (bytes)
gap            DB    ?                 ; GAP para lectura/escritura
byte1          DB    ?                 ; bits HD, US1, US0
relleno        DB    ?                 ; byte de relleno (al escribir)

buffer         EQU   $                 ; para leer/escribir sector

fdc_test       ENDS
               END   main
