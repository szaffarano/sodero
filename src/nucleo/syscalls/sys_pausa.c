#include <sodero/syscalls.h>

/**
 * sys_pausa: System call encargada de realizar una pausa en el proceso en
 * ejecucion en ese momento.
 *
 * @return estado_fin 0 por OK.
 */
int sys_pausa () {
   //Ejecuta la IRQ0. Obliga una pausa por timer. Si no es apropiativo se
   //maneja la logica igualmente en esta interrupcion.
   __asm__ __volatile__ ( "int $0x20\n" );
   return 0;
}
