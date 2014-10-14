#include <video.h>
#include <sodero/memoria.h>
#include <timer.h>
#include <dir.h>
#include <salir.h>
#include <sodero/syscalls.h>

extern int listar_directorio ();

/**
 * Ejecutable encargado de listar el contenido del directorio raiz
 */
int main (int cant_arg, char** arg) {
   listar_directorio ();
   salir (0);
   imprimir ( "si ves esto algo anda mal!!!\n" );
   return 0;
}
