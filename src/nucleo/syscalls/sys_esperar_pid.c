#include <sodero/syscalls.h>
#include <sodero/proceso.h>
#include <sodero/gdt.h>
#include <timer.h>

/**
 * Puntero al inicio de la lista de procesos
 */
extern nodo* lista_ready_inicio;

/**
 * La tarea que se esta ejecutando en este momento
 */
extern pcb* tarea_actual;

/**
 * sys_esperar_pid: Recorre la cola de listos hasta que el pid del proceso
 * con el cual fue llamada deje de existir en la Cola Listos o en ejecucion.
 * O sea muera.
 *
 * @param pid pid del proceso a esperar.
 *
 * @return estado_fin pid del proceso que espero que muera esperar. -1 por ERROR
 */
int sys_esperar_pid ( int pid ) {
   static nodo* tmp;
   int pid_lectura = -1;
   int flag = 1;

   if ( lista_ready_inicio == NULL ) {
      return -1; //Si Cola listos vacia devuelve ERROR.
   }

   while ( flag ) {
      tmp = lista_ready_inicio;
      cli ();
      do {
         pid_lectura = tmp->tarea->pid; //Se carga el pid del proceso de la
                                        //cola que va recorriendo.
         tmp = (nodo*) tmp->anterior; //Se mueve en la cola.
      } while ( tmp != NULL && pid != pid_lectura ); //Mientras no sea fin de
                                                     //la cola y pid no igual.
      sti ();
      //permanezco bucleando mientras la tarea que esperamos que muera 
      //permanezca en ejecucion o en la cola de ready
      flag = pid == pid_lectura || tarea_actual->pid == pid;
   }
   return pid;
}
