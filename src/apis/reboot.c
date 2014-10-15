#include <sodero/syscalls.h>
#include <reboot.h>

/**
 * reboot: api que realiza la llamada a la system call que resetea la PC
 *
 * @return (int) exito de la operacion
 */
int reboot ( ) {
   return llamar_sys_call ( SYS_REBOOT, 0, 0, 0 );
}
