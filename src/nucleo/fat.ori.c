/*
 * Modulo que implemeta la logica de lectura del file system FAT12
 */
#include <sodero/disquete.h>
#include <sodero/syscalls.h>
#include <fat.h>

// funciones locales al modulo
//static void imprimir_entrada (entrada_directorio*);
static int buscar_entrada ( char*, entrada_directorio* );
static int cluster_a_lba ( int );
static int comparar_nombres ( char*, char* );

int leer_archivo ( int fd, void* buffer, unsigned int cant );

extern int comparar_cadenas ( const char*, const char* );

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
 * Puntero a la tarea que esta ejecutando actualmente.
 */
extern pcb* tarea_actual;

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

/**
 * buscar_entrada: busca una entrada dentro del root directory
 * @param nombre nombre de la entrada a buscar
 * @param arch puntero a la variable donde se almacenaran los datos
 *        de la entrada encontrada
 *
 * @return el estado de la operacion
 */
int buscar_entrada ( char* nombre, entrada_directorio* arch ) {
   int indice, j;
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

   // leo los N sectores del directorio raiz
   for ( indice = 0; indice < tamanio_dir_raiz; indice++ ) {
      if ( leer_sector_logico ( sector_dir_raiz++, ed ) == LECTURA_ERRONEA ) {
         imprimir ( "Error leyendo directorio raiz!\n" );
         return ERROR;
      }
      for ( j = 0; j < cant_entradas; j++ ) {
         if ( ed[j].nombre[0] == ULTIMA_ENTRADA ) {
            return ERROR;
         } else if ( ed[j].atributos != ATR_NOMBRE_LARGO && 
                     ed[j].atributos != ATR_ID_UNIDAD &&
                     ed[j].nombre[0] != BORRADO ) {
            if ( comparar_nombres ( nombre, ed[j].nombre ) == OK ) {
               *arch = ed[j];
               return OK;
            }
         }
      }
   }
   return ERROR;
}


/**
 * comparar_nombres: Compara un nombre de archivo ingresado con otro de la 
 *                   entrada del root directory.  Por ejemplo:
 *                   busqueda = "ARCH.TXT"
 *                   archivo = "ARCH    TXT"
 * @param busqueda nombre del archivo a comparar
 * @param archivo nombre del archivo en la entrada directorio
 */
static int comparar_nombres ( char* busqueda, char* archivo ) {
   int indice = 0;
   int indice_nuevo;
   char archivo_nuevo[9];

   while ( archivo [indice] != ' ' && indice < 8 ) {
      archivo_nuevo [indice] = archivo [indice++];
   }

   if ( archivo [8] != ' ' ) {
      indice_nuevo = indice;
      indice = 8;
      archivo_nuevo [indice_nuevo++] = '.';
      while ( archivo [indice] != ' ' && indice < 11 ) {
         archivo_nuevo [indice_nuevo++] = archivo [indice++];
      }
      indice = indice_nuevo;
   }
   archivo_nuevo [indice] = '\0';

   return !comparar_cadenas ( archivo_nuevo, busqueda );
}

int proximo ( int cluster ) {
   int flag = ( (cluster & 1) == 1 ); // si es impar
   cluster = cluster * 3 / 2;
   /*
    * BUG!!!! El C del ORRTOO al no castear (fat+cluster) lo tomaba como un
    * char* entonces el HDP solamente le asignaba a cluster los primeros 8 bits
    * por eso se podian ver solamente algunos sectores (en particular se podian
    * ver los clusters menores a 0xF).  Casteandolo a word, es decir a una
    * variable de 16 bits, se asignan esos 2 bytes.
    * PROBLEMA RESUELTO!!!!!!!!!! :-))
    */
   cluster = *(word*)( fat + cluster );
   if ( flag ) {
      cluster >>= 4;
   }
   return (cluster & 0x0FFF);
}

/**
 * abrir: Abre un archivo para lectura, devuelve el descriptor de archivo.
 * @param nombre nombre del archivo a abrir
 *
 * @return un file descriptor representando al archivo o -1 en caso de error
 */
int abrir ( char* nombre ) {
   int indice;

   entrada_directorio ed;

   if ( fat == NULL ) {
      if ( cargar_fat () == ERROR ) {
         imprimir ("Error cargando la tabla FAT\n");
         return ERROR;
      }
   }

   if ( buscar_entrada ( nombre, &ed ) == ERROR ) {
      //imprimir ( "El archivo no existe!\n" );
      return ERROR;
   }

   indice = tarea_actual->siguiente_archivo++;

   tarea_actual->archivos [ indice ].ed = ed;
   tarea_actual->archivos [ indice ].cluster_actual = ed.primer_cluster_bajo;
   tarea_actual->archivos [ indice ].posicion_actual = 0;

   return indice;
}

/**
 * leer_archivo: lee desde un file descriptor, que representa un archivo, 
 *               analoga a la funcion read de UNIX
 * @param fd descriptor de archivo
 * @param buffer posicion de memoria donde se leera
 * @param cant cantidad de bytes a leer
 * @return estado de la lectura
 */
int leer_archivo ( int fd, void* buffer, unsigned int cant ) {
   unsigned int cluster_a_leer;
   unsigned int tam_lectura;
   char* buffer_temporal;
   int sector_logico;
   int bytes_leidos = 0;
   int indice_copia = 0;
   int idx;
   descriptor_archivo descr = tarea_actual->archivos [ fd ];

   #ifdef DEBUG
   imprimir ( "Se va a leer: <" );
   for ( idx = 0; idx < 11; idx++ ) {
      imprimir ( "%c", descr.ed.nombre[idx] );
   }
   imprimir ( ">\n" );
   #endif

   buffer_temporal = (char*) sys_alocar ( 512 );

   indice_copia = descr.posicion_actual % 512;
   while ( cant > 0 ) {
      // si todavia tengo que leer el sector actual (aun no avanzo).
      if ( descr.posicion_actual == 0 || (descr.posicion_actual % 512) != 0 ) {
         cluster_a_leer = descr.cluster_actual;
         if ( cant < ( 512 - (descr.posicion_actual % 512) ) ) {
            tam_lectura = cant;
         } else {
            tam_lectura = 512 - ( descr.posicion_actual % 512 );
         }
      } else {
         cluster_a_leer = proximo ( descr.cluster_actual );
         descr.cluster_actual = cluster_a_leer;
         if ( cluster_a_leer == 0x0 || (cluster_a_leer & 0x0FF0) == 0x0FF0 ) {
            imprimir ( "ultimo cluster: 0x%x\n", cluster_a_leer );
            //return bytes_leidos;
            break;
         }
         tam_lectura = cant > 512 ? 512 : cant;
      }

      if ( tam_lectura + bytes_leidos > descr.ed.tamanio ) {
         tam_lectura = descr.ed.tamanio - bytes_leidos;
         tam_lectura = tam_lectura > 512 ? 512 : tam_lectura;
         cant = 0;
         imprimir ("se quiere leer mas de lo que se puede!\n" );
      }

      sector_logico = cluster_a_lba ( cluster_a_leer );
      leer_sector_logico ( sector_logico, buffer_temporal );

      for ( idx = indice_copia; idx < (indice_copia+tam_lectura); idx++ ) {
         ((char*)buffer) [ idx - indice_copia + bytes_leidos ] = 
                                                        buffer_temporal [ idx ];
      }

      cant -= tam_lectura;
      descr.posicion_actual += tam_lectura;
      bytes_leidos += tam_lectura;
      indice_copia += tam_lectura;
      indice_copia = indice_copia == 512 ? 0 : indice_copia;
   }
   tarea_actual->archivos [ fd ] = descr;

   return bytes_leidos;
}

/** 
 * cluster_a_lba: transforma un cluster (direccion logica de FAT) a un
 *                sector LBA (direccion logica de sector)
 * @param cluster cluster a transformar
 * @return el sectoren formato LBA
 */
int cluster_a_lba ( int cluster ) {
   int sector_dir_raiz;
   int tamanio_dir_raiz;
   int inicio;

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
    */
   tamanio_dir_raiz = ( sector_booteo->BPB_cant_entradas_root * 
                        sizeof ( entrada_directorio ) ) / 512;

   inicio = tamanio_dir_raiz + sector_dir_raiz;

   return ( (cluster - 2) * sector_booteo->BPB_sectores_por_cluster ) + inicio;
}
