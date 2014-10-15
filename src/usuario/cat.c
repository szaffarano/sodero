#include <es.h>
#include <video.h>
#include <salir.h>

#define MAX_BUFF 512
int main ( int argc, char* argv[] ) {
   int fd, i;
   char* buff = (char*) alocar ( MAX_BUFF + 1 );

   if ( argc != 2 ) {
      imprimir_cadena ( "Error en los argumentos\n" );
      imprimir_cadena ( "\t Utilizar cat <nombre_archivo>\n" );
      salir ( 1 );
   }

   fd = abrir ( argv[1] );
   if ( fd < 0 ) {
      imprimir_cadena ( "Error al abrir " );
      imprimir_cadena ( argv[1] );
      imprimir_cadena ( "\n" );
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
