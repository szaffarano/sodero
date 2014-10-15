#include <sodero/disquete.h>
#include <sodero/syscalls.h>
#include <fat.h>
#include <dir.h>

extern BPB* sector_booteo; // el BPB
extern int leer_sector_logico ( int, void* );

extern int recuperar_base ();

int sys_leer_entrada_dir ( char*, dir* );
int sys_abrir_dir ( dir* );

int sys_dir () {
   dir* directorio = NULL;

   char* dato = (char*) alocar ( 256 );

   imprimir_cadena ( "\nListado del directorio raiz:\n" );

   if ( sys_abrir_dir ( directorio ) == -1 ) {
      imprimir_cadena ( "Error al abrir dir!\n" );
      sys_salir (1);
   } 

   while ( sys_leer_entrada_dir ( dato, directorio ) != NULL ) {
      imprimir_cadena ( dato );
   }
   return 0;
}

int sys_abrir_dir ( dir* directorio ) {
   int sector_dir_raiz;
   int tamanio_dir_raiz;
   int cant_entradas = 512 / sizeof(entrada_directorio);

   //dir* directorio = direc + recuperar_base ();

   if ( sector_booteo == NULL ) {
      if ( cargar_BPB (sector_booteo) == ERROR ) {
         imprimir_cadena ("Error cargando el sector de booteo\n");
         return ERROR;
      }
   }

   /**
    * calculo el primer sector del directorio raiz: 
    * 1 (BPB) + cant_sectores_reservados + tam_FAT * cant_tablas_FAT
    */
   sector_dir_raiz = sector_booteo->BPB_cant_sectores_reservados + 1 +
                  sector_booteo->BPB_tamanio_FAT * sector_booteo->BPB_cant_FAT; 

   /**
    * calculo la cantidad de sectores que ocupa el directorio raiz
    * Nota: se redondea para arriba!
    */
   tamanio_dir_raiz = ( sector_booteo->BPB_cant_entradas_root * 
                        sizeof ( entrada_directorio ) + 512 ) / 512;


   directorio->sector_a_leer = sector_dir_raiz;
   directorio->indice = -1;
   directorio->tamanio_dir_raiz = tamanio_dir_raiz;
   directorio->cant_entradas = cant_entradas;
   imprimir_cadena ("\ncantidad de sectores a leer: ");
   imprimir_entero (directorio->sector_a_leer);
   imprimir_cadena ("\nindice: ");
   imprimir_entero (directorio->indice);
   imprimir_cadena ("\ntamanio dir raiz: ");
   imprimir_entero (directorio->tamanio_dir_raiz);
   imprimir_cadena ("\ncantidad de entradas: ");
   imprimir_entero (directorio->cant_entradas);

   return OK;
}

int sys_leer_entrada_dir ( char* entrada, dir* directorio ) {
   int i;
   static entrada_directorio* ed = NULL;

   //dir* directorio = direc + recuperar_base ();
   //char* entrada = ent + recuperar_base ();

   if ( directorio == NULL ) {
      return ERROR;
   }

   if ( ed == NULL ) {
      ed = (entrada_directorio*) sys_alocar ( directorio->cant_entradas );
   }

   if ( directorio->indice == -1 ) {
      if ( leer_sector_logico (directorio->sector_a_leer++, ed) == LECTURA_ERRONEA ) {
         imprimir ( "Error leyendo directorio raiz!\n" );
         return ERROR;
      }
      directorio->indice = 0;
   }

   if ( ed[directorio->indice].nombre[0] == ULTIMA_ENTRADA ) {
      return NULL;
   }

   for ( i = 0; i < 11; i++ ) {
      entrada[i] = ed[directorio->indice].nombre[i];
   }
   entrada[i] = '\0';

   directorio->indice = directorio->indice + 1 == directorio->cant_entradas ? 
                        -1 : ++directorio->indice;

   return OK;
}
