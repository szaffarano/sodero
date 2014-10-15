#include <timer.h>
#include <sodero/registros.h>
#include <sodero/memoria.h>
#include <video.h>
#include <sodero/idt.h>
#include <sodero/cdrom.h>
#include <fat.h>
#include <sodero/disquete.h>
#include <sodero/proceso.h>
#include <sodero/gdt.h>
#include <ejecutar.h>
#include <esperar.h>

void a_mayusculas ( char* );
void a_minusculas ( char* );

#define PROMPT "sodero# "
#define MAX_ARGS 10
#define MAX_LENGTH_ARGS 255

static int parsear_argumentos ( char* comando, 
                                char argumentos [MAX_ARGS][MAX_LENGTH_ARGS] );

/**
 * main: proceso de usuario que se encarga de hacer de interfaz con el usuario
 * se encarga de leer y parsear los datos que se ingresan por teclado.  
 * Llama a la API ejecutar cuando es necesario. 
 *
 * @param cant_arg (int) cantidad de argumentos
 *
 * @param arg (char**) lista de argumentos
 *
 * @return (int) exito de la operacion
 */
int main (int argc, char* argv[]) {
   int i, j, cant_args;
   int pid;
   char* dato = NULL;
   char argumentos [MAX_ARGS][MAX_LENGTH_ARGS];
   char* args_param[MAX_ARGS];
   //indica si la tarea que quiero levantar debe correr en segundo plano o no
   int sp = 0;  
   int largo;

   dato = (char*) alocar (LIMITE);

   do {
      setear_memoria ( dato, '\0', LIMITE );
      for ( i = 0; i < MAX_ARGS; i++ ) {
         setear_memoria ( argumentos[i], '\0', MAX_LENGTH_ARGS );
      }

      imprimir (PROMPT);
      ingresar_cadena (dato);
      cant_args = parsear_argumentos ( dato, argumentos );

      for ( i = 0; i < MAX_ARGS; i++ ) {
         args_param[i] = (char*) alocar ( MAX_LENGTH_ARGS );
         setear_memoria ( args_param[i], '\0', MAX_LENGTH_ARGS );
      }

      i = 0;
      while ( argumentos[i][0] != NULL ) {
         j = 0;
         while ( argumentos[i][j] != '\0' ) {
            args_param[i][j] = argumentos[i][j] ;
            j++;
         }
         args_param[i][j] = '\0' ;
         i++;
      }
      args_param[i] = NULL;

      sp = 0;
      if ( cant_args > 0 ) {
         largo = longitud_cadena ( dato );
         if ( dato [largo - 2] == '&' ) {
            dato [largo - 2] = '\0';
            sp = 1;
         }
         a_mayusculas ( dato );
         pid = ejecutar ( dato, args_param );
         if ( pid == -1 ) {
            imprimir ( "comando desconocido\n" );
         } else if (sp == 0) {
            esperar_pid ( pid );
         }
      }
   } while (1);
   return 0;
}

/**
 * parsear_argumentos: Funcion que se encarga de parsear los argumentos, o sea
 * de separar cuando encuentra un espacio (' ') los argumentos y asignarlos al
 * array de argumantos. 
 *
 * @param comando dato ingresado por teclado por el usuario
 *
 * @param argumentos[] direccion del array de argumentos donde se van a setear
 *                     los mismos luego del parseo. 
 *
 * Ej: Recibe: 'ls -l -a' 
 * antes:
 * comando ls -l -a
 * despues
 * comando = ls
 * argumentos[0] = -l
 * argumentos[1] = -a
 * argumentos[2] = NULL
 */
static int parsear_argumentos ( char* comando,
                                char argumentos [MAX_ARGS][MAX_LENGTH_ARGS] ) {
   int i = 0;
   int j = 0;
   int k = 0;

   if ( comando[0] == '\0' ) {
      return 0;
   }

   do {
      k = 0;
      while ( comando [i] != ' ' && comando [i] != '\0' ) {
         argumentos[j][k++] = comando [i];
         i++;
      }
      comando[i] = '\0';
      argumentos[j++][k] = '\0';
      i++;
   } while ( comando[i] != '\0' );
   argumentos[j][0] = NULL;
   return j;
}

/**
 * a_minusculas: Funcion que se encarga de pasar una cadena a minusculas
 *
 * @param min cadena a convertir a minusculas. 
 */
void a_minusculas ( char* min ) {
   int idx = 0;
   while ( min[idx] != '\0' ) {
      min[idx++] |= 0x20;
   }
}

/**
 * a_mayusculas: Funcion que se encarga de pasar una cadena a mayusculas
 *
 * @param may cadena a convertir a mayusculas. 
 */
void a_mayusculas ( char* may ) {
   int idx = 0;
   while ( may[idx] != '\0' ) {
      may[idx++] &= 0xDF;
   }
}
