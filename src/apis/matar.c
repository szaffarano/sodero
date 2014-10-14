#include <sodero/syscalls.h>
#include <matar.h>

/**
 * matar: API que se utiliza para terminar o finalizar un proceso que se 
 * encuentre activo en el sistema. Recibe como parametro el pid del proceso
 * a matar. Se llama a la System call sys_matar.
 *
 * @param pid pid del proceso a matar
 *
 * @return estado_fin pid del proceso que se mato por OK. -1 por pid invalido.
 *                    -2 por pid corresponde a proceso sistema IMPOSIBLE matar.
 */
int matar ( int pid ) {
   return llamar_sys_call ( SYS_MATAR, pid, 0, 0 );
}
