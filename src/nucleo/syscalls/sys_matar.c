#include <sodero/syscalls.h>
#include <sodero/proceso.h>

/**
 * Puntero al inicio de la lista de procesos
 */
extern nodo* lista_ready_inicio;

/**
 * sys_matar: System call encargada de matar un proceso. Recibe un pid del
 * proceso y lo mata.
 * 
 * @param pid Pid del proceso a matar.
 *
 * @return estado_fin pid del proceso matado por OK, -1 pid invalido,
 *                    -2 proceso de sistema, imposible matar.
 */
int sys_matar ( int pid ) {
   static nodo* tmp;
   int ret = 0;

   if ( lista_ready_inicio == NULL ) { //Si cola vacia.
      ret = -1;
   } else {
      tmp = lista_ready_inicio; //Apunto al comienzo de la cola de listos.
      cli ();
      do {
         if ( pid == tmp->tarea->pid) { //Si es el pid buscado
            if ( tmp->tarea->tipo == PROCESO_USUARIO ) { //Si es proceso de
                                                         //de USUARIO.
               ret = pid;
               tmp->tarea->estado = KILL; //Mato proceso
            } else {
               ret = -2; //Si es sistema no se puede matar.
            }
            break;
         }
         tmp = (nodo*) tmp->anterior;
      } while ( tmp != NULL ); //Recorro cola hasta encontra el proceso.
      sti ();
   }
   return ret;
}
