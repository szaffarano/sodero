/*
 * Modulo que implemeta la logica de lectura del file system FAT12
 */
#include <sodero/disquete.h>
#include <sodero/syscalls.h>
#include <fat.h>

#ifdef DEBUG
static void dump_BPB ();
#endif

/**
 * El sector numero cero del disco (Bios Parameter Block)
 */
BPB* sector_booteo; // el BPB

/**
 * La tabla de FAT ( donde se encuentra la informacion de los clusters )
 */
byte* fat; // la tabla FAT

/**
 * leer_sector_logico: lee un sector logico del disquete
 *
 * @param sector_logico sector en formato LBA a leer
 * @param buffer direccion de memoria donde se lee
 *
 * @return estado de la lectura
 */
int leer_sector_logico ( int sector_logico, void* buffer ) {
   dword pista, cabeza, sector;

   word spp = sector_booteo->BPB_sectores_por_pista;
   word nc = sector_booteo->BPB_numero_cabezas;

   pista = ( sector_logico - 1 ) / ( spp * nc );
   cabeza = 1 & ( ( sector_logico - 1 ) / spp );
   sector = 1 + ( ( sector_logico - 1 ) % spp);

   return leer_sector ( cabeza, pista, sector, buffer );
}

/**
 * cargar_BPB: Carga el BIOS Parameter Block en memoria (en una variable global)
 * @param sector_booteo direccion en memoria de la variable conteniendo los
 *                      datos a leer
 *
 * @return el estado de la operacion
 */
int cargar_BPB (BPB* sector_booteo) {
   char* buffer;

   buffer = (char*) sys_alocar (512);

   if ( leer_sector ( 0, 0, 1, buffer ) == LECTURA_ERRONEA ) {
      return ERROR;
   }

   copiar_memoria ( buffer, sector_booteo, sizeof (BPB) );

   return OK;
}

/**
 * cargar_fat: Carga la tabla FAT en memoria (en una variable global)
 *
 * @return el estado de la operacion
 */
int cargar_fat() {
   int indice;
   int tamanio_fat;
   dword sector_fat;

   if ( sector_booteo == NULL ) {
      if ( cargar_BPB (sector_booteo) == ERROR ) {
         imprimir ("Error cargando el sector de booteo\n");
         return ERROR;
      }
   }

   // obtengo el tamanio de la fat en bytes
   tamanio_fat = sector_booteo->BPB_tamanio_FAT * 512; 

   // le aloco memoria
   fat = (byte*) sys_alocar ( tamanio_fat );

   // inicializo la FAT con ceros
   setear_memoria ( fat, 0, tamanio_fat );

   // obtengo el primer sector (LOGICO) donde comienza la tabla FAT
   // (el primer sector es el del BPB, luego del cual vienen los sectores
   // reservados, y a continuacion la tabla FAT).
   sector_fat = sector_booteo->BPB_cant_sectores_reservados + 1;

   // leo los N sectores que conforman la tabla FAT
   imprimir ( "Cargando FAT" );
   for ( indice = 0; indice < sector_booteo->BPB_tamanio_FAT; indice++ ) {
      imprimir ( "." );
      /*
       * sector_fat es un sector logico, el cual antes de leerlo hay que
       * trasnformarlo a CHS.
       * fat es el buffer donde leemos, pero hay que ir incrementandolo por
       * cada lectura(la primera vez (fat), la segunda (fat+512), y asi...
       */
      if ( leer_sector_logico (sector_fat, fat + indice * 512) == 
                                                        LECTURA_ERRONEA ) {
         imprimir ( "Error al leer la tabla fat!\n" );
         return ERROR;
      }
      // leo el siguiente sector logico
      sector_fat++;
   }
   imprimir ( "\n" );
   #ifdef DEBUG
   // el MediaID es 0xF0 para disquettes!
   imprimir ( "FAT Cargada, MediaID: 0x%xb\n", *fat );
   #endif

   return OK;
}

/**
 * inicializa_fat: Setea el BPB y la FAT en NULL para que actuen como flags 
 *                 (asi se cargan una unica vez)
 */
void inicializa_fat () {
   sector_booteo = NULL;
   fat = NULL;
}

/**
 * test_fat: Funcion de prueba utilizada durante la etapa de desarrollo.
 */
void test_fat() {

   if ( cargar_fat () == ERROR ) {
      imprimir ("Error cargando la tabla FAT\n");
      return;
   }

   #ifdef DEBUG
   dump_BPB();
   #endif
}

/**
 * Muestra el contenido del BPB
 */
#ifdef DEBUG
static void dump_BPB () {
   sector_booteo->BS_nombre_OEM[7] = '\0';
   imprimir ( "nombre_OEM: '%s'\n", sector_booteo->BS_nombre_OEM );
   imprimir ( "bytes_por_sector: 0x%xw\n",sector_booteo->BPB_bytes_por_sector );
   imprimir ( "sec_cluster: 0x%xb\n", sector_booteo->BPB_sectores_por_cluster );

   imprimir ("sec_resev: 0x%xw\n", sector_booteo->BPB_cant_sectores_reservados);
   imprimir ( "cant_FAT: 0x%xb\n", sector_booteo->BPB_cant_FAT );
   imprimir ( "entr_root: 0x%xw\n", sector_booteo->BPB_cant_entradas_root );

   imprimir ( "cant_sectores: 0x%xw\n",sector_booteo->BPB_cant_sectores_FAT16 );
   imprimir ( "media: 0x%xb\n", sector_booteo->BPB_media );
   imprimir ( "tamanio_FAT: 0x%xw\n", sector_booteo->BPB_tamanio_FAT );

   imprimir ( "sect_pista: 0x%xw\n", sector_booteo->BPB_sectores_por_pista );
   imprimir ( "nro_cabezas: 0x%xw\n", sector_booteo->BPB_numero_cabezas );
   imprimir ( "sect_ocultos: 0x%x\n", sector_booteo->BPB_sectores_ocultos );

   imprimir ( "sect_FAT32: 0x%x\n", sector_booteo->BPB_total_sectores_FAT32 );
   imprimir ( "nro_disp: 0x%xb\n", sector_booteo->BS_numero_dispositivo );
   imprimir ( "reservado: 0x%xb\n", sector_booteo->BS_reservado );

   imprimir ( "firma: 0x%xb\n", sector_booteo->BS_bootsig );
   imprimir ( "id_volumen: 0x%x\n", sector_booteo->BS_id_volumen );

   sector_booteo->BS_etiqueta_volumen [10] = '\0'; 
   imprimir ( "etiqueta: %s\n", sector_booteo->BS_etiqueta_volumen );

   sector_booteo->BS_tipo_filesystem [7] = '\0'; 
   imprimir ( "tipo_filesystem: %s\n", sector_booteo->BS_tipo_filesystem );
}
#endif
