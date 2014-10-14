#include <sodero/syscalls.h>
#include <video.h>

/**
 * ejecutar: API que se utiliza para ejecutar un proceso a partir de un archivo
 * Se llama a la System Call sys_ejecutar.
 *
 * @param path Nombre del archivo a ejecutar.
 *
 * @return estado_fin pid del nuevo proceso por OK. -1 por ERROR.
 */
int ejecutar ( char* path, char* args[] ) {
   return llamar_sys_call ( SYS_EJECUTAR, (dword)path, (dword) args, 0 );
}
