#ifndef _TSS_H_
#define _TSS_H_

/**
 * sodero/tss.h
 * biblioteca con todas las definiciones de datos y funciones relacionadas
 * con el manejo de tareas. 
 */

#include <sodero/definiciones.h>
#include <sodero/registros.h>
#include <sodero/segmento.h>

/**
 * Definicion de tipo que representa un puntero a funcion (utilizada
 * para setear el EIP de un TSS con el valor de la tarea que tendra dicho TSS)
 * Nota: Se utiliza el seteo del EIP con este valor siempre y cuando no sea un
 * proceso de usuario. En este caso el EIP es igual a 0 (cero) y el comienzo
 * del segmento de codigo de usuario posee este valor donde comienza la 
 * funcion de usuario propiamente dicha.
 */
typedef void (*handler_tarea)(void);

/**
 * Un TSS: representa el estado de una tarea en un momento dado.
 */
typedef struct {
  word back_link; // 2 bytes
  word empty0; // 2 bytes
  dword esp0; // 4 bytes
  word ss0; // 2 bytes
  word empty1; // 2 bytes
  dword esp1; // 4 bytes
  word ss1; // 2 bytes
  word empty2; // 2 bytes
  dword esp2; // 4 bytes
  word ss2; // 2 bytes
  word empty3; // 2 bytes
  dword cr3; // 4 bytes
  dword eip; // 4 bytes
  dword eflags; // 4 bytes
  dword eax; // 4 bytes
  dword ecx; // 4 bytes
  dword edx; // 4 bytes
  dword ebx; // 4 bytes
  dword esp; // 4 bytes
  dword ebp; // 4 bytes
  dword esi; // 4 bytes
  dword edi; // 4 bytes
  word es; // 2 bytes
  word empty4; // 2 bytes
  word cs; // 2 bytes
  word empty5; // 2 bytes
  word ss; // 2 bytes
  word empty6; // 2 bytes
  word ds; // 2 bytes
  word empty7; // 2 bytes
  word fs; // 2 bytes
  word empty8; // 2 bytes
  word gs; // 2 bytes
  word empty9; // 2 bytes
  word LDTSegmentSelector; // 2 bytes
  word empty10; // 2 bytes
  unsigned debugTrapFlag:1; // 1 bit
  unsigned empty11:15; // 15 bits
  word IOMapBaseAddress; // 2 bytes
} NOALIGN tss; // tamanio total = 104 bytes

/**
 * Descriptor de un TSS: variable que 'vive' en la GDT y que
 * referenciara a un TSS dado.
 */
typedef struct {
  word limite_0_15;
  word base_0_15;
  byte base_16_23;
  unsigned tipo_1:1; // este bit va siempre en uno
  unsigned flag_busy:1;
  unsigned tipo_2:2; // estos dos bits van seteados a dos -> 10b
  unsigned cero:1; // este bit va siempre seteado a cero
  unsigned dpl:2; // descriptor privilege level
  unsigned presente:1; // si el segmento esta o no presente
  unsigned limite_16_19:4;
  unsigned disponible:1; // si esta disponible para usar por soft de sistema (?)
  unsigned ceros:2; // siempre va 00
  unsigned granularidad:1; // idem que para los segmentos
  byte base_24_31;
} NOALIGN tss_desc;

/**
 * Esta funcion realiza el ltr (Load TR, Cargar Registro de Tarea)
 * Se invoca una unica vez en la ejecucion del Sistema operativo para
 * setear el TR con la tarea de sistema.
 *
 * @param selector Selector de la tarea de sistema (init)
 */
void cargar_tr (const word);

/**
 * Realiza un far jmp a la tarea referenciada por el selector que recibe
 * como argumento.
 * 
 * @param selector Selector de la tarea a ejecutar. 
 *
 * @param offset desplazamiento a partir del eip que esta contenido en la TSS
 *              apuntada por el descriptor dentro de la GDT de TSS en caso
 *              que el comienzo de la funcion principal de la tarea no se
 *              encuentre al comienzo del eip referenciado.
 */
void ir_a_tarea (const word selector, const dword offset);

/**
 * Setea a cero un TSS dado.
 *
 * @param tss direccion de memoria donde se encuentra la TSS a setear.
 */
void inicializar_tss (tss*);

/**
 * Setea un TSS dado con los valores necesarios para comenzar su ejecucion
 * (selectores de segmento, eflags, etc. mas el EIP apuntando a la
 * primer instruccion a ejecutar).
 * 
 * @param tss Direccion de memoria del TSS a setear
 *
 * @param handler_tarea Direccion de la funcion principal de la tarea a setear 
 */
void setear_tarea (tss*, const handler_tarea);

#endif // _TSS_H_
