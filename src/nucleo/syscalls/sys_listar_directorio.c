/*
 * Modulo de la sys call que lista un directorio (imprimiendo sus entradas)
 */
#include <sodero/syscalls.h>
#include <sodero/proceso.h>
#include <fat.h>
#include <sodero/disquete.h>
#include <sodero/disquete.h>

/**
 * El Bios Parameter Block
 */
extern BPB* sector_booteo; // el BPB

/**
 * La tabla FAT
 */
extern byte* fat; // la tabla FAT

/**
 * Seteo de colores (para imprimir la salida)
 */
extern int sys_setear_color ( byte );

static int imprimir_entrada (entrada_directorio* arch);

/**
 * sys_listar_directorio: leer el root directory para mostrar su contenido
 * @return el estado de la operacion
 */
int sys_listar_directorio () {
   int indice, j;
   int bytes_totales = 0;
   int cant_arch = 0;
   int sector_dir_raiz;
   int tamanio_dir_raiz;
   int cant_entradas = 512 / sizeof(entrada_directorio);
   entrada_directorio ed[cant_entradas];

   if ( sector_booteo == NULL ) {
      if ( cargar_BPB (sector_booteo) == ERROR ) {
         imprimir ("Error cargando el sector de booteo\n");
         return ERROR;
      }
   }

   /*
    * calculo el primer sector del directorio raiz: 
    * 1 (BPB) + cant_sectores_reservados + tam_FAT * cant_tablas_FAT
    */
   sector_dir_raiz = sector_booteo->BPB_cant_sectores_reservados + 1 +
                  sector_booteo->BPB_tamanio_FAT * sector_booteo->BPB_cant_FAT; 

   /*
    * calculo la cantidad de sectores que ocupa el directorio raiz
    * Nota: se redondea para arriba!
    */
   tamanio_dir_raiz = ( sector_booteo->BPB_cant_entradas_root * 
                        sizeof ( entrada_directorio ) + 512 ) / 512;

   //imprimo encabezado.
   if ( tamanio_dir_raiz > 0 ) {
      imprimir ( "Listando directorio a:\\\n" );
   }

   // leo los N sectores del directorio raiz
   for ( indice = 0; indice < tamanio_dir_raiz; indice++ ) {
      if ( leer_sector_logico ( sector_dir_raiz++, ed ) == LECTURA_ERRONEA ) {
         imprimir ( "Error leyendo directorio raiz!\n" );
         return ERROR;
      }
      for ( j = 0; j < cant_entradas; j++ ) {
        if ( ed[j].nombre[0] == ULTIMA_ENTRADA ) {
           imprimir ( "Cantidad de archivos: %d\n", cant_arch );
           imprimir ( "Total ocupado: %d bytes\n", bytes_totales );
           return OK;
        }
        //cant_arch += imprimir_entrada ( &ed[j] );
        if (imprimir_entrada ( &ed[j] ) == 1) {
           cant_arch++;
           if ( ed[j].atributos != ATR_DIRECTORIO ) {
              bytes_totales += ed[j].tamanio;
           }
        }
      }
   }
   return ERROR;
}

/**
 * imprimir_entrada: Funcion privada al modulo que formatea una entrada 
 *                   directorio para mostrarla bien.
 * @param arch entrada de directorio a imprimir
 *
 * @return el estado de la operacion
 */
static int imprimir_entrada (entrada_directorio* arch) {
   int indice;
   int contador = 0;
   if ( arch->atributos != ATR_NOMBRE_LARGO && 
        arch->atributos != ATR_ID_UNIDAD &&
        arch->nombre[0] != BORRADO ) {

      sys_setear_color (arch->atributos == ATR_DIRECTORIO ? AZUL_CLARO : 
                                                            VERDE_CLARO);
      for ( indice = 0; indice < 8 && arch->nombre[indice] != ' '; indice++ ) {
         imprimir ( "%c", arch->nombre[indice] );
         contador++;
      }
      if ( arch->nombre[8] != ' ' ) {
         imprimir ( "." );
         for ( indice = 8; indice < 11; indice++ ) {
            imprimir ( "%c", arch->nombre[indice] );
            contador++;
         }
      }
      sys_setear_color ( BLANCO );

      if ( contador < 7 ) {
         imprimir ("\t");
      }

      if ( arch->atributos == ATR_DIRECTORIO ) {
         sys_setear_color ( AZUL_CLARO );
         imprimir ("\t\t<DIRECTORIO>\n");
         sys_setear_color ( BLANCO );
      } else  {
         sys_setear_color ( VERDE_CLARO );
         imprimir ( "\t\t%d\tbytes\n", arch->tamanio );
         sys_setear_color ( BLANCO );
      }
      return 1;
   }
   return 0;
}
