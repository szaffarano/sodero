#ifndef _ADMIN_H
#define _ADMIN_H

/**
 * sodero admin_procesos.h
 * Biblioteca que contiene todo el manejo del scheduler. Administra los
 * procesos y contiene los datos necesarios para este manejo.
 */
 
#include <sodero/definiciones.h>

/**
 * Enum que identifica el tipo de algoritmo utilizado por el scheduler.
 */ 
typedef enum {
    FIFO,
    LIFO,
    PRIO,      //no apropiativo (quantum)
    STF,
    GTF,
    SODERO,
    RND,
    GPF,
    LPF
} algoritmo_proceso_id;

/**
 * Esta funcion representa a la tarea del scheduler
 */
void admin_procesos ();

/**
 * Esta funcion representa a la tarea del timer (IRQ 0)
 */
void timer_handler (dword);

#endif // _ADMIN_H
