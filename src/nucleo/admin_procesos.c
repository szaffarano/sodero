#include <sodero/admin_procesos.h>
#include <sodero/proceso.h>
#include <sodero/definiciones.h>
#include <timer.h>
#include <sodero/puertos.h>

//#define DEBUG

/**
 * Contador para comparar con el QUANTUM
 */
int contador;

/**
 * Tarea que esta ejecutando
 **/
pcb* tarea_actual = NULL;

/**
 * PCB que representa el scheduler (necesitamos tenerla declarada para
 * cuando hagamos el context switch desde el timer, IRQ0)
 */
pcb* pcb_scheduler;

/**
 * Puntero a la GDT
 **/
extern gdt* _gdt;

/**
 * Modo apropiacion  (0: no apropiativo - 1: apropiativo)
 **/
int apropiativo = 0;

/**
 * admin_procesos: esta funcion centraliza la administracion de procesos, 
 * volviendo a la cola de procesos listos, el proceso que se encuentra en 
 * ejecucion, siempre que no haya terminado, y poniendo en ejecucion al 
 * proximo proceso de la cola
 *
 * @see agregar()
 * @see sacar()
 */
void admin_procesos () {
   pcb* aux;

   cli();

   while (1) {
      //almaceno la actual
      aux = tarea_actual;
   
      //obtengo la tarea que debe ser ejecutada
      if ((tarea_actual = sacar ()) != NULL) {
         //imprimir ("tarea nueva: %d\n", tarea_actual->pid);
   
         //dejo el rastro
         tarea_actual->ultima_tarea = (struct pcb*)aux;
   
         if ( aux != NULL ) {
            if ( aux->estado == READY ) {
               //imprimir ("Pasando a READY la tarea %x...\n", aux->pid);
               agregar (aux);
            } else if ( aux->estado == WAIT ) {
               //imprimir ("Pasando a WAIT la tarea %x...\n", aux->pid);
               // aun no implementado!
               // agregar_wait (aux);
            } else if ( aux->estado == KILL ) {
               //imprimir ("Pasando a KILL la tarea %d...\n", aux->pid);
               //deberia liberar la memoria utilizada por la tarea matada
            }
         }
   
         _gdt->descriptores[tarea_actual->selector].desc_tss.flag_busy = 0;
         ir_a_tarea ( tarea_actual->selector * 8, 0 );
      }
   }
}

/**
 * timer_handler: Funcion encargada de atender a la IRQ 0
 *
 * @param numero (dword) numero de interrupcion que atiende
 */
void timer_handler (dword numero) {
   while (1) {
      outb (0x20, 0x20);
      if ( apropiativo == 0) {
         contador++;
         if (contador >= QUANTUM) {
            contador = 0;
            tarea_actual->ts_ejec += QUANTUM * 1000 / 18;
            _gdt->descriptores[pcb_scheduler->selector].desc_tss.flag_busy = 0;
            ir_a_tarea ( pcb_scheduler->selector * 8, 0 );
         }
      }
   }
}
