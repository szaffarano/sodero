#include <sodero/syscalls.h>
#include <esperar.h>

/**
 * esperar_pid: API que se utiliza para que un proceso espere que termine otro
 * y luego recien continuar su ejecucion. Recibe el pid del proceso a esperar 
 * como parametro. Se llama a la system call sys_ejecutar_pid
 *
 * @param pid pid del proceso a esperar.
 *
 * @return estado_fin pid del proceso que espere por OK. -1 por ERROR.
 */
int esperar_pid ( int pid ) {
   return llamar_sys_call ( SYS_ESPERAR_PID, pid, 0, 0 );
}
