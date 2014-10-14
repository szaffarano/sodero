#include <sodero/tareas.h>
#include <sodero/proceso.h>
#include <video.h>
#include <sodero/idt.h>
#include <timer.h>

/**
 * Puntero a la tarea que esta ejecutando actualmente.
 */
extern pcb* tarea_actual;

/**
 * Tarea oseosa que va imprimiendo su propio nombre y su PID.
 */
void tarea_dummy() {
   imprimir ("%c", *(tarea_actual->dato++));
   //PAUSA
   while (*(tarea_actual->dato) != '\0') {
      delay (MICRO(50000L));
      imprimir ("%c", *(tarea_actual->dato++));
      //PAUSA
   }
   imprimir ("%d", tarea_actual->pid);
}

