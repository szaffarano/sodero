#include <ejecutar.h>
#include <salir.h>
#include <es.h>
#include <video.h>

#define ARCHIVO_AYUDA "AYUDA.TXT"
#define MAX_BUFF 512

int main ( int argc, char* argv[] ) {
   int fd, i;
   char* buff = (char*) alocar ( MAX_BUFF + 1 );

   fd = abrir ( ARCHIVO_AYUDA );
   if ( fd < 0 ) {
      imprimir_cadena ( "Imposible mostrar la ayuda\n" );
      salir ( 1 );
   }

   while ( leer ( fd, buff, MAX_BUFF ) > 0 ) {
      buff [MAX_BUFF] = '\0';
      imprimir_cadena ( buff );
      for ( i = 0; i <= MAX_BUFF; i++ ) {
         buff[i] = '\0';
      }
   }
   salir ( 0 );
   return 0;
}
