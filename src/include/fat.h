#ifndef _FAT_H_
#define _FAT_H_

#include <sodero/definiciones.h>

// atributos de los archivos
#define ATR_SOLO_LECTURA 0x01
#define ATR_OCULTO 0x02
#define ATR_SISTEMA 0x04
#define ATR_ID_UNIDAD 0x08
#define ATR_DIRECTORIO 0x10
#define ATR_ARCHIVO 0x20
#define ATR_NOMBRE_LARGO  ( ATR_SOLO_LECTURA | ATR_OCULTO | \
                            ATR_SISTEMA | ATR_ID_UNIDAD )

#define ULTIMA_ENTRADA 0x00
#define BORRADO 0xE5

typedef struct {
   byte BS_salto_a_boot [3]; // jmp al inicio de booteo del disco.
   byte BS_nombre_OEM [8]; //Nombre del sistema Operativo.
   word BPB_bytes_por_sector;
   byte BPB_sectores_por_cluster; //Un cluster = una unidad de FAT
   word BPB_cant_sectores_reservados; //No debe ser 0. Siempre 0x01,
                                      //reservado FAT32.
   byte BPB_cant_FAT; //total de tablas FAT en el disco.
   word BPB_cant_entradas_root; //Cantidad de entradas en el directorio Raiz.
   word BPB_cant_sectores_FAT16; //cantidad de sectores del disco. 
                                  //Para FAT32 debe ser 0
   byte BPB_media; //0xF8 si es fijo | 0xF0 si es extraible (disquete)
   word BPB_tamanio_FAT; //Tamanio de la tabla FAT en Sectores.
   word BPB_sectores_por_pista;
   word BPB_numero_cabezas; //En disquete de 3.5 de 1.44Mb valor=0x02
   dword BPB_sectores_ocultos; //0 para unidades no particionadas.
   dword BPB_total_sectores_FAT32; //0 si es FAT12 o 16. Total sectores
                                   //de disco para FAT32
   byte BS_numero_dispositivo; //0x80 Disco Rigido | 0x00 disquete.
   byte BS_reservado; //Siempre 0x00. -- Solo usado en NT.
   byte BS_bootsig; //0x29 indica que existen los tres campos
                    //que siguen en la BPB
   dword BS_id_volumen;
   byte BS_etiqueta_volumen [11];
   byte BS_tipo_filesystem [8]; //Ej: "FAT12   " o "FAT16   " o "FAT     "
} NOALIGN BPB; //Solo para FAT12 o FAT16 //BIOS PARAMETERS BLOCK

typedef struct {
   unsigned char nombre[11];
   byte atributos;
   byte reservado;
   byte ts_creacion; // en milisegundos
   word hora_creacion;
   word fecha_creacion;
   word fecha_ultimo_acceso;
   word primer_cluster_alto; // los 2 bytes mas significativos (0 para FAT12)
   word hora_ultima_escritura;
   word fecha_ultima_escritura;
   word primer_cluster_bajo; // los 2 bytes menos significativos
   dword tamanio;
} NOALIGN entrada_directorio;

void inicializa_fat ();
int cargar_BPB (BPB*);
void test_fat ();
int listar_directorio_raiz ();
int cargar_fat ();
int leer_archivo ( int, void*, unsigned int );
int abrir ( char* nombre );
int leer_sector_logico ( int, void* );

#endif // _FAT_H_
