#include <xchg_aprop.h>

extern int apropiativo;

/**
 * sys_xchg_aprop: handler de la system call que permite cambiar el modo de 
 * apropiacion del microprocesador
 *
 * @return (int) exito de la operacion
 */
int sys_xchg_aprop ( ) {
   apropiativo = !apropiativo;
   return apropiativo;
}
