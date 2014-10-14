#include <sodero/init.h>
#include <sodero/syscalls.h>
#include <sodero/admin_procesos.h>
#include <video.h>
#include <timer.h>
#include <sodero/proceso.h>

/**
 * Definicion del nombre del ejecutable del proceso Interfaz
 */
#define INTERFAZ "INTERFAZ"


/**
 * Modo de apropiacion  (0: no apropiativo - 1: apropiativo)
 */
extern int apropiativo;

/**
 * PCB que representa el scheduler (necesitamos tenerla declarada para
 * cuando hagamos el context switch desde el timer, IRQ0)
 */
extern pcb* pcb_scheduler;

/**
 * Puntero a la GDT
 */
extern gdt* _gdt;

/**
 * Tarea que esta ejecutando
 **/
extern pcb* tarea_actual;

/**
 * Funcion init. Funcion - tarea del sistema que encarga de contabilizar el
 * tiempo de ejecucion de cada proceso que corren en el sistema. Deriva el
 * control al Scheduler o administrador de procesos.
 */
void init () {
   while (1) {
      if (apropiativo == 1) {
        tarea_actual->ts_ejec += QUANTUM * CLOCK;
        _gdt->descriptores[pcb_scheduler->selector].desc_tss.flag_busy = 0;
        ir_a_tarea ( pcb_scheduler->selector * 8, 0 );
      } else {
         sys_pausa ();
      }
   }
}
