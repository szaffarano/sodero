#include <ps.h>

/**
 * Puntero al inicio de la lista de procesos
 */
extern nodo* lista_ready_inicio;

/**
 * Puntero a la tarea actual
 */
extern pcb* tarea_actual;

/**
 * sys_ps: System Call que lista los procesos que se estan ejecutando en el
 * sistema operativo en ese momento.
 *
 * @return estado_fin 0 por OK. 
 */
int sys_ps ( ) {
   static nodo* tmp;

   imprimir_cadena ( "\tProcesos Corriendo\n" );
   imprimir_cadena ( "\t~~~~~~~~ ~~~~~~~~~\n" );
   imprimir_cadena ( "Pid:\t\tNombre\t\t\tTiempo de ejecucion (Seg.)\n" );
   tmp = lista_ready_inicio;
   while ( tmp != NULL ) {
      imprimir_entero ( tmp->tarea->pid );
      imprimir_cadena ("-->\t\t");
      if ( tmp->tarea == NULL || tmp->tarea->dato == NULL ) {
         imprimir_cadena ( "?\t\t\t?" );
      } else {
         imprimir_cadena ( tmp->tarea->dato );
         if ( longitud_cadena ( tmp->tarea->dato ) < 7 ) {
            imprimir_cadena ( "\t" );
         }
         imprimir_cadena ( "\t\t" );
         imprimir_entero ( ( tmp->tarea->ts_ejec / 1000 ) );
      }
      imprimir_cadena ("\n");
      tmp = (nodo*) tmp->anterior;
   }
   imprimir_entero ( tarea_actual->pid );
   imprimir_cadena ("-->\t\t");
   if ( tarea_actual == NULL || tarea_actual->dato == NULL ) {
      imprimir_cadena ( "?\t\t\t?" );
   } else {
      imprimir_cadena ( tarea_actual->dato );
      if ( longitud_cadena ( tarea_actual->dato ) < 7 ) {
         imprimir_cadena ( "\t" );
      }
      imprimir_cadena ( "\t\t" );
      imprimir_entero ( ( tarea_actual->ts_ejec / 1000 ) );
   }
   imprimir_cadena ("\n");

   return 0;
}
