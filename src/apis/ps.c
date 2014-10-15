#include <sodero/syscalls.h>
#include <ps.h>

/**
 * ps: api que realiza la llamada a la system call que muestra los procesos
 *
 * @return (int) exito de la operacion
 */
int ps ( ) {
   return llamar_sys_call ( SYS_PS, 0, 0, 0 );
}
