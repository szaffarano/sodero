#ifndef _PROCESO_H_
#define _PROCESO_H_

/**
 * sodero/proceso.h
 * biblioteca con todas las definiciones de datos y funciones relacionadas
 * con el manejo de procesos.
 */

#include <sodero/definiciones.h>
#include <sodero/tss.h>
#include <sodero/idt.h>
#include <sodero/gdt.h>
#include <video.h>
#include <sodero/memoria.h>
#include <fat.h>

/**
 * Enum para identificar un proceso es de usuario o de sistema. Es una entrada
 * mas en la pcb.
 */ 
typedef enum {
   PROCESO_USUARIO,
   PROCESO_SISTEMA
} tipo_proceso;

/**
 * Enum el estado de un proceso en un momento determinado. Es una entrada
 * mas en la pcb.
 */ 
typedef enum {
   READY,
   WAIT,
   EXEC,
   KILL
} estado_proceso;

/**
 * Estructura que representa un descriptor de Archivo. su entrada de directorio 
 * su cluster actual y su posicion actual del ARCHIVO.
 */
typedef struct {
   entrada_directorio ed;
   int fin_de_archivo;
   unsigned int cluster_actual;
   unsigned int bytes_leidos;
   unsigned int posicion_actual; // en bytes
} descriptor_archivo;

/**
 * Estructura que representa una PCB. Describe al proceso. Su estado, tipo,
 * tiempo en ejecucion (uso de CPU), PID, etc.
 */
typedef struct {
     tss* tarea;
     word selector;
     word selector_ds;
     dword ts_ejec;
     dword pid;
     byte prioridad;
     estado_proceso estado;
     struct pcb* ultima_tarea;
     char* dato;
     descriptor_archivo archivos [10];
     int estado_retorno; // el codigo de retorno del proceso
     int siguiente_archivo;
     tipo_proceso tipo;
} pcb;

/**
 * Estructura que representa un nodo de la cola y que posee el contenido de un
 * PCB y un puntero al siguiente nodo y uno al anterior y conformar asi una
 * lista doblemente enlazada.
 */
typedef struct {
    struct nodo* anterior;
    pcb* tarea;
    struct nodo* siguiente;
} nodo;

/**
 * Puntero a la GDT
 */
extern gdt* _gdt;

/**
 * Funcion que crea un nuevo nodo para el manejo de colas del sistema.
 *
 * @return nodo Nuevo nodo creado con su alocacion de memoria correspondiente.
 */
nodo* nuevonodo ();

/**
 * Funcion que agrega un proceso en la cola de listos.
 *
 * @param pcb proceso a agregar en la cola de listos. 
 */
void agregar (const pcb*);

/**
 * Funcion que saca un proceso de la cola de listos.
 *
 * @return pcb proceso que se elimino de la cola de listos. 
 */
pcb* sacar ();

/**
 * Funcion que saca un proceso de la cola de listos.
 *
 * @param tarea direccion de la funcion principal de la tarea a crear
 *
 * @return pcb nuevo proceso.
 */
pcb* crear_tarea (const handler_tarea);

/**
 * Funcion que realiza un dump de la cola de listos. Muestra el contenido de
 * la cola de LISTOS. 
 */
void dump_cola_ready ();

#endif //_PROCESO_H_
