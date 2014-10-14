#include <xchg_aprop.h>

/**
 * xchg_aprop: api que realiza la llaamda a la system call que 
 * alterna el modo de apriacion del procesador (apropiativo - no apropiativo)
 *
 * @return (int) exito de la operacion
 */
int xchg_aprop ( ) {
   return llamar_sys_call ( SYS_XCHG_APROP, 0, 0, 0 );
}
