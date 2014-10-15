#include <video.h>
#include <salir.h>
#include <matar.h>
#include <video.h>
#include <sodero/syscalls.h>

/**
 * Ejecutable encargado de interrumpir la ejecucion de un proceso que se
 * encuentra corriendo.
 */
int main (int argc, char* argv[]) {
   int pid;
   int rta;

   if ( argc != 2 ) {
      imprimir_cadena ( "Error en los argumentos\n" );
      imprimir_cadena ( "\t Utilizar matar <numero_pid>\n" );
      salir ( 1 );
   }

   pid = ctoi (argv[1]);
   if ( pid == NULL ) {
      imprimir_cadena ("pid incorrecto\n");
      salir (1);
   }

   rta = matar (pid);
   if ( rta == -1 ) {
       imprimir_cadena ( "error en matar\n" );
   } else if ( rta == -2 ) {
       imprimir_cadena ( "imposible matar un proceso de sistema\n" );
   } else if (rta == pid) {
       imprimir_cadena ( "proceso interceptado y eliminado de la cola\n" );
   } else {
       imprimir_cadena ( "no se encontro al proceso en la cola\n" );
   }

   salir (2);
   imprimir_cadena ( "si ves esto algo anda mal!!!\n" );
   return 0;
}
