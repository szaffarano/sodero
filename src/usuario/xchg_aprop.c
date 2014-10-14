#include <video.h>
#include <salir.h>
#include <sodero/syscalls.h>
#include <xchg_aprop.h>

/**
 * main: proceso que alterna el modo de apropiacion del microprocesador
 * a traves de la api correspondiente (@see xchg_aprop.c)
 *
 * @param cant_arg (int) cantidad de argumentos
 *
 * @param arg (char**) lista de argumentos
 *
 * @return (int) exito de la operacion
 */
int main (int cant_arg, char** arg) {
   int rta;
   rta = xchg_aprop ();
   if ( rta == -1 ) {
       imprimir_cadena ( "\n\nerror cambiando el modo de apropiacion del procesador \n" );
   } else {
       imprimir_cadena ("\n --- modo de apropiacion: ");
       if ( rta == 0 ) {
          imprimir_cadena ("no apropiativo\n");
       } else {
          imprimir_cadena ("apropiativo\n");
       }
   }

   salir (2);  //obliga la finalizacion del proceso
               //su estado pasa a KILL, no puede volver a la cola de ready
   imprimir_cadena ( "si ves esto algo anda mal!!!\n" );
   return 0;
}
