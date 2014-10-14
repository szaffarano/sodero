#include <sodero/syscalls.h>
#include <salir.h>

/**
 * salir: API que se utiliza para que termine o finalice el proceso actual.
 * Recibe como parametro el valor con el cual finaliza el proceso
 * o que retorna luego de que concluya su ejecucion. Se llama a la
 * System call sys_salir.
 *
 * @param retorno valor de retorno con cual concluye el proceso actual.
 *
 * @return estado_fin 0 por OK. -1 por ERROR.
 */
int salir ( int retorno ) {
   return llamar_sys_call ( SYS_SALIR, retorno, 0, 0 );
}
