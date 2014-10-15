/**
 * Este modulo se encarga de todo el manejo de los dispositivos IDE
 */

#include <sodero/registros.h>
#include <sodero/syscalls.h>
#include <timer.h>
#include <sodero/puertos.h>
#include <sodero/interrupciones.h>
#include <video.h>
#include <sodero/ide.h>
#include <sodero/memoria.h>

//#define DEBUG 1

volatile word flag_irq_ide;

/**
 * Arreglo de descriptores de los 4 posibles dispositivos ide
 */
descriptor_ide descs_ide[4];

static char* get_tipo(byte);

/**
 * inicializar_ide: Funcion que se encarga de inicializar los dispositivos 
 * IDE. Inicializa los 4 posibles dispositivos. Primario Master, primario
 * esclavo, secundario master, secundarios esclavo, en caso de que existan. 
 */
void inicializar_ide () {
   int i;
   imprimir ("Inicializando dispositivos IDE...\n");

   descs_ide[0].puerto_base = BASE_PRIMARIO;
   descs_ide[1].puerto_base = BASE_PRIMARIO;
   descs_ide[2].puerto_base = BASE_SECUNDARIO;
   descs_ide[3].puerto_base = BASE_SECUNDARIO;

   descs_ide[0].id_dispositivo = ID_MASTER;
   descs_ide[1].id_dispositivo = ID_ESCLAVO;
   descs_ide[2].id_dispositivo = ID_MASTER;
   descs_ide[3].id_dispositivo = ID_ESCLAVO;

   descs_ide[0].presente = detectar_ide ( BASE_PRIMARIO );
   descs_ide[1].presente = descs_ide[0].presente;

   descs_ide[2].presente = detectar_ide ( BASE_SECUNDARIO );
   descs_ide[3].presente = descs_ide[0].presente;

   habilitar_dispositivo ( BASE_PRIMARIO );
   habilitar_dispositivo ( BASE_SECUNDARIO );

   for ( i = 0; i < 4; i++ ) {
      descs_ide[i].tipo = detectar_dispositivo ( descs_ide[i].puerto_base,
                                                 descs_ide[i].id_dispositivo );
   }

   if ( descs_ide[0].presente == PRESENTE ) {
      imprimir ( "Se detecto el IDE1 (0x%xw)\n", descs_ide[0].puerto_base);
      imprimir("\tEl ide0 maestro es %s\n", get_tipo(descs_ide[0].tipo));
      imprimir("\tEl ide0 esclavo es %s\n", get_tipo(descs_ide[1].tipo));
   }

   if ( descs_ide[2].presente == PRESENTE ) {
      imprimir ( "Se detecto el IDE2 (0x%xw)\n", descs_ide[2].puerto_base);
      imprimir("\tEl ide1 maestro es %s\n", get_tipo(descs_ide[2].tipo));
      imprimir("\tEl ide1 esclavo es %s\n", get_tipo(descs_ide[3].tipo));
   }
}

/**
 * detectar_ide: Funcion que se encarga de detecta la existacia de un 
 * determinado dispositivo IDE.
 *
 * @param puerto puerto que representa el tipo de dispositivo IDE. Ya sea
 *               Maestro o esclavo, primario o secundario.     
 */
int detectar_ide (const word puerto) {
   byte lect_cont, lect_sect;

   outb ( 0x55, puerto + REG_CONTADOR );
   outb ( 0xAA, puerto + REG_SECTOR );

   lect_cont = inb ( puerto + REG_CONTADOR );
   lect_sect = inb ( puerto + REG_SECTOR );

   /*
   if ( (lect_cont == 0x55 && lect_sect == 0xAA) ||
        (lect_cont == 0x00 && lect_sect == 0x00) ) {
   */

   if ( (lect_cont == 0x55 && lect_sect == 0xAA) ) {
      #ifdef DEBUG
      imprimir ( "Se encontro un controlador IDE en 0x%xw\n", puerto );
      #endif
      return PRESENTE;
   } else {
      #ifdef DEBUG
      #endif
      imprimir ( "Controlador IDE no presente 0x%xw 0x%xb 0x%xb\n", puerto,
                                                                    lect_cont,
                                                                    lect_sect );
   }
   return AUSENTE;
}

int habilitar_dispositivo ( const word puerto ) {
   outb ( 0x06, puerto + REG_CONTROL ); // comando para resetear el dispositivo
   delay ( MICRO(4000L) );

   outb ( 0x00, puerto + REG_CONTROL ); // se habilitan las IRQs para el disp.
   delay ( MICRO(4000L) );

   if ( leer_estado ( puerto, 0xC1, 0x40, 50 ) ) {
      #ifdef DEBUG
      #endif
      imprimir ( "El dispositivo 0x%xw no respondio a tiempo\n", puerto );
      return 0;
   }
   return 1;
}

int detectar_dispositivo ( const word puerto, const byte id ) {
   byte tipo;

   if ( seleccionar_dispositivo ( puerto, id ) ) {
      if ( (tipo = identificar_dispositivo (puerto)) == DESCONOCIDO ) {
         tipo = identificar_dispositivo (puerto);
      }
   }
   return tipo;
}

int leer_estado ( const word puerto, const word mascara_estado,
                  const word bits_estado, dword reintentos ) {

   byte estado;

   while ( reintentos-- > 0 ) {
      estado = inb ( puerto + REG_ESTADO );
      if ( (estado & mascara_estado) == bits_estado ) {
         return 0;
      }
      delay ( MICRO(10000L) );
   }
   return 1;
}

int identificar_dispositivo ( const word puerto ) {
   byte lectura_lsb, lectura_msb, estado;
   byte tipo;
   ide_id id;

   if ( inb (puerto + REG_CONTADOR) != 0x01 ||
        inb (puerto + REG_SECTOR) != 0x01 ) {
      imprimir ("Error al intentar identificar a 0x%xw\n", puerto);
      return DESCONOCIDO;
   }

   lectura_lsb = inb ( puerto + REG_LSB_CYL );
   lectura_msb = inb ( puerto + REG_MSB_CYL );
   estado = inb ( puerto + REG_ESTADO );

   if ( lectura_lsb == 0x14 && lectura_msb == 0xEB ) {
      #ifdef DEBUG
      imprimir ( "\tSe encontro una unidad de CD-ROM en 0x%xw\n", puerto );
      #endif
      tipo = CDROM;
      outb ( COMANDO_ID_CDROM, puerto + REG_COMANDO );
   } else if ( lectura_lsb == 0 && lectura_msb == 0 && estado != 0x00 ) {
      #ifdef DEBUG
      imprimir ( "\tSe encontro un disco rigido en 0x%xw\n", puerto );
      #endif
      tipo = HD;
      outb ( COMANDO_ID_HD, puerto + REG_COMANDO );
   } else {
      #ifdef DEBUG
      imprimir ( "\tNo se reconoce el dispositivo IDE: 0x%xb 0x%xb 0x%xb\n", 
                  lectura_lsb, lectura_msb, estado );
      #endif
      return DESCONOCIDO;
   }

   delay ( MICRO(40000L) );

   if ( esperar_irq ( 0xC000, 2000 ) == 0 ) {
      imprimir ( "No se produjo la IRQ\n" );
      return DESCONOCIDO;
   }

   leer_puerto ( puerto + REG_DATOS, (word*)&id, 256 );

   #ifdef DEBUG
   imprimir ( "Numero de modelo: %s\n", obtener_cadena ((word*)&id, 27, 40) );
   imprimir ( "Numero de serie: %s\n", obtener_cadena ((word*)&id, 10, 20) );
   imprimir ( "Firmware: %s\n", obtener_cadena ((word*)&id, 23, 8) );
   #endif

   return tipo;
}

int esperar_irq ( const int mascara, unsigned int reintentos ) {
   while ( reintentos-- > 0 ) {
      if ( (mascara & flag_irq_ide) != 0 ) {
         flag_irq_ide &= ~(mascara & flag_irq_ide);
         return 1;
      }
      delay ( MICRO(100L) );
   }
   return 0;
}

void irq14_handler (int numero) {
   #ifdef DEBUG
   imprimir ("se disparo la IRQ14\n");
   #endif

   flag_irq_ide |= MASCARA_IRQ14;
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

void irq15_handler (int numero) {
   #ifdef DEBUG
   imprimir ("se disparo la IRQ15\n");
   #endif

   flag_irq_ide |= MASCARA_IRQ15;
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

void leer_puerto ( const word puerto, word* buffer, unsigned int tamanio ) {
   while ( tamanio-- > 0 ) {
       *buffer = inw ( puerto );
       buffer++;
   }
}

void escribir_puerto ( const word puerto, word* buffer, unsigned int tamanio ) {
   while ( tamanio-- > 0 ) {
       outw ( *buffer, puerto );
       buffer++;
   }
}

word* obtener_cadena ( word* buffer, unsigned int inicio, unsigned int tam ) {
   int i;
   word datos[256];
   word c;
   word* ret;
   word* p;

   ret = (word*) sys_alocar ( tam );

   setear_memoria (datos, 0, tam);
   for ( i = inicio; i < (inicio+tam); i++ ) {
      *datos = *(buffer+i);
   }

   for ( i = 0; i < tam; i += 2 ) {
      c = datos[i];
      datos[i] = datos[i+1];
      datos[i+1] = c;
   }

   i = tam - 2;

   while ( datos[i--] == ' ' );

   datos [i+1] = '\0';

   p = (word*) datos;

   while ( *p++ == 0x20 );

   i = 0;

   while ( *p != '\0' ) {
      ret[i++] = *p++;

   }
   ret[i] = '\0';

   return ret;
}

int seleccionar_dispositivo ( word puerto, byte id ) {
   outb ( id, puerto + REG_SEL_DRIVER );
   delay ( MICRO(40000L) );

   return !leer_estado ( puerto, 0x88, 0x00, 50 );
}

static char* get_tipo(byte tipo) {
   switch ( tipo ) {
      case CDROM:
         return "una unidad de CDROM";
      case HD:
         return "un disco rigido";
   }
   return "desconocido!";
}
