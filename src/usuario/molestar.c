#include <video.h>
#include <salir.h>
#include <sodero/syscalls.h>
#include <speaker.h>
#include <timer.h>

/**
 * main: Este proceso de usuario realiza hace sonar el speaker cada un segundo
 * Por cierto, esto la hace realmente molesta.
 */
int main (int cant_arg, char** arg) {
   while (1) {
      delay (MICRO(1000000L));
      beep (1000, 1000);
   }

   salir (2);
   imprimir_cadena ( "si ves esto algo anda mal!!!\n" );
   return 0;
}
