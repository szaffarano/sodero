#include <sodero/syscalls.h>
#include <sodero/proceso.h>


/**
 * La tarea que se esta ejecutando en este momento
 */
extern pcb* tarea_actual;

/**
 * sys_salir: System Call que se encarga de salir del (matar el) proceso actual.
 * 
 * @param retorno estado de finalizacion del proceso que termina.
 *
 * @return estado_fin Devuelve 0 por OK.
 */
int sys_salir ( int retorno ) {
   tarea_actual->estado = KILL;
   tarea_actual->estado_retorno = retorno;
   sys_pausa ();
   return 0;
}
