#include <sodero/sys_es.h>

static int buscar_entrada ( char*, entrada_directorio* );
static int comparar_nombres ( char*, char* );
static int cluster_a_lba ( int );
static int proximo ( int cluster );

extern int comparar_cadenas ( const char*, const char* );
extern int sys_alocar ( const dword );
extern int recuperar_base ();

/**
 * Puntero a la tarea que esta ejecutando actualmente.
 */
extern pcb* tarea_actual;

/**
 * El sector numero cero del disco (Bios Parameter Block)
 */
extern BPB* sector_booteo; // el BPB

/**
 * La tabla de FAT ( donde se encuentra la informacion de los clusters )
 */
extern byte* fat; // la tabla FAT

/**
 * sys_abrir: Abre un archivo para lectura, devuelve el descriptor de archivo.
 * @param nombre nombre del archivo a abrir
 *
 * @return un file descriptor representando al archivo o -1 en caso de error
 */
int sys_abrir ( char* archivo ) {
   int indice;
   char* nombre = archivo + recuperar_base ();

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
   tarea_actual->archivos [ indice ].fin_de_archivo = 0;
   tarea_actual->archivos [ indice ].bytes_leidos = 0;

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
int sys_leer ( int fd, void* buff, unsigned int cant ) {
   unsigned int cluster_a_leer;
   unsigned int tam_lectura;
   char* buffer_temporal;
   int sector_logico;
   int bytes_leidos = 0;
   int indice_copia = 0;
   int posicion_inicial = 0;
   int idx;
   char* buffer = NULL;
   descriptor_archivo descr = tarea_actual->archivos [ fd ];

   if ( descr.fin_de_archivo == 1 ) {
      return -1;
   }

   buffer = buff + recuperar_base ();

   #ifdef DEBUG
   imprimir ( "Se va a leer: <" );
   for ( idx = 0; idx < 11; idx++ ) {
      imprimir ( "%c", descr.ed.nombre[idx] );
   }
   imprimir ( ">\n" );
   #endif

   buffer_temporal = (char*) sys_alocar ( 512 );

   indice_copia = descr.posicion_actual % 512;
   posicion_inicial = descr.posicion_actual;
   while ( cant > 0 && descr.fin_de_archivo == 0 ) {
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
            descr.fin_de_archivo = 1;
            break;
         }
         tam_lectura = cant > 512 ? 512 : cant;
      }

      if ( posicion_inicial + tam_lectura + bytes_leidos > descr.ed.tamanio ) {
         #ifdef DEBUG
         imprimir ( "\ntam_lect: %d - bytes_leidos: %d - tamanio: %d\n",
                    tam_lectura, bytes_leidos, descr.ed.tamanio );
         #endif

         tam_lectura = descr.ed.tamanio - descr.bytes_leidos;
         tam_lectura = tam_lectura > 512 ? 512 : tam_lectura;
         descr.fin_de_archivo = 2;

         #ifdef DEBUG
         imprimir ("se quiere leer mas de lo que se puede!\n" );
         imprimir ( "indice_copia: %d - tam_lectura: %d - cant: %d\n",
                    indice_copia, tam_lectura, cant );
         #endif
      }

      sector_logico = cluster_a_lba ( cluster_a_leer );
      leer_sector_logico ( sector_logico, buffer_temporal );

      for ( idx = indice_copia; idx < (indice_copia+tam_lectura); idx++ ) {
         ((char*)buffer) [ idx - indice_copia + bytes_leidos ] = 
                                                        buffer_temporal [ idx ];
      }
      ((char*)buffer) [ idx - indice_copia + bytes_leidos + 1 ] = '\0';

      cant -= tam_lectura;
      descr.posicion_actual += tam_lectura;
      bytes_leidos += tam_lectura;
      descr.bytes_leidos += tam_lectura;
      indice_copia += tam_lectura;
      indice_copia = indice_copia == 512 ? 0 : indice_copia;
   }
   tarea_actual->archivos [ fd ] = descr;

   if ( descr.fin_de_archivo == 1 ) {
      return -1;
   }

   if ( descr.fin_de_archivo == 2 ) {
      descr.fin_de_archivo = 1;
   }

   return bytes_leidos;
}

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
