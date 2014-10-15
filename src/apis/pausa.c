#include <sodero/syscalls.h>
#include <esperar.h>

/**
 * pausa: API que se utiliza para relizar una pausa en un proceso en ejecucion.
 * Esto provoca que el proceso actual vuelva a la cola de listos.
 * Se llama a la System Call sys_pausa.
 *
 * @return estado_fin 0 por OK.
 */
int pausa () {
   return llamar_sys_call ( SYS_PAUSA, 0, 0, 0 );
}
