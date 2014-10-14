#include <video.h>
#include <salir.h>
#include <sodero/syscalls.h>
#include <reboot.h>

/**
 * main: proceso de usuario que invoca a la api de resetear la pc
 *
 * @param cant_arg (int) cantidad de argumentos
 *
 * @param arg (char**) lista de argumentos
 *
 * @return (int) exito de la operacion
 */
int main (int cant_arg, char** arg) {
   if ( reboot () != 0 ) {
       imprimir_cadena ( "\n\nerror en reboot\n" );
   }

   salir (2); //aqui se obliga la finalizacion del proceso, su estado pasa
              //a KILL, por lo tanto no puede volver a la cola de ready
   imprimir_cadena ( "si ves esto algo anda mal!!!\n" );
   return 0;
}
