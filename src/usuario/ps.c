#include <video.h>
#include <salir.h>
#include <sodero/syscalls.h>
#include <ps.h>

/**
 * main: proceso de usuario que invoca a la api de mostrar procesos activos
 * en el sistema. Este proceso se encarga de listar los procesos activos.
 *
 * @param cant_arg (int) cantidad de argumentos
 *
 * @param arg (char**) lista de argumentos
 *
 * @return (int) exito de la operacion
 */
int main (int cant_arg, char* arg[]) {
   if ( ps () != 0 ) {
       imprimir_cadena ( "\n\nerror en ps\n" );
   }

   salir (2);
   imprimir_cadena ( "si ves esto algo anda mal!!!\n" );
   return 0;
}
