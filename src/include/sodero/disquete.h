#ifndef _DISKETE_H_
#define _DISKETE_H_

#include <sodero/puertos.h>
#include <sodero/registros.h>
#include <sodero/dma.h>
#include <sodero/memoria.h>
#include <video.h>
#include <timer.h>
#include <sodero/proceso.h>
#include <sodero/tareas.h>

/**
 * <pre>
 * El FDC (me estoy basando en el 82077AA de Intel, el cual _CREO_ que es
 * compatible con el de NEC), tiene 9 registros como interface con el micro,
 * los cuales van desde 0x3F0 a 0x3F7:
 *
 * SRA: Registro de Estado Primario (Status Register A)(no accesible desde PC)
 * SRB: Registro de Estado Secundario (Status Register B)(no accesible desde PC)
 *
 * DOR: Registro de Salida Digital (Digital Output Register)
 * TDR: Registro del dispositivo de disco (Tape Driver Register) +-> ta bien 
 *      la traduccion?? ;-)
 * MSR: Registro Principal de Estado (Main Status Register)
 * DSR: Registro Selector de Datos (Data Rate Select Register)
 * FIFO: Registro de Datos
 * DIR: Registro de Entrada Digital (Digital Input Register)
 * CCR: Registro de Control de Configuracion (Configuration Control Registrer)
 *
 * Estructura del DOR (Digital Output Register)
 *  +----+----+----+----+-----+---+----+----+
 *  | M1 | M2 | M1 | M0 | DMA | R | S1 | S2 |
 *  +----+----+----+----+-----+---+----+----+
 *    |    |    |    |     |    |    |    |
 *    |    |    |    |     |    |    +----+--> Unidad a seleccionar
 *    |    |    |    |     |    |
 *    |    |    |    |     |    +--> Resetear unidad ( en 0 resetea )
 *    |    |    |    |     |
 *    |    |    |    |     +--> Si 1 DMA activa. Tener en cuenta que setear
 *    |    |    |    |          este bit a cero implica deshabilitar la IRQ6.
 *    |    |    |    |
 *    +----+----+----+--> Encender/Apagar motor
 *
 * Estructura del MSR (Main Status Register)
 *  +-----+-----+-----+-----+------+------+------+------+
 *  | RQM | DIO | DMA | CMD | DRV3 | DRV2 | DRV1 | DRV0 |
 *  |     |     |     | BSY | BSY  | BSY  | BSY  | BSY  |
 *  +-----+-----+-----+-----+------+------+------+------+
 *  RQM: si esta seteado a uno, entonces el FDC puede transferir datos, si
 *       esta seteado a cero, no se permite el acceso.
 *  DIO: al setearse el RQM indica la direccion en la que se van a transferir
 *       los datos: 1 indica que hay que leer al FDC y 0 que hay que escribir.
 *  DMA: Aca va el valor que se especifico en el comando SPECIFY, se seteara
 *       unicamente durante la fase de ejecucion de un comando.
 *  CMD BUSY: se setea a uno cuando un comando esta en progeso, se activa cuando
 *            el FDC acepta el comando, y se desactiva el finalizar la fase
 *            de resultado.
 *  DRVn BUSY: se setea a uno cuando una unidad esta realizando un seek (se
 *             da en los comandos seek y recalibrate)
 *
 * Estructura del FIFO: Segun el manual es un registro de 16 bits!!!
 *                      se utiliza tanto para leer/escribir datos como
 *                      los parametros de los comandos.  Para acceder al
 *                      mismo (ya sea leyendo o escribiendo), es importante
 *                      tener en cuenta los bits RQM y DIO del MSR.
 * </pre>
 */

/**
 * Base del puerto del controlador de disquette.
 */
#define FDC_BASE 0x3F0

// definicion de los puertos contra los cuales vamos a escribit/leer

#define FDC_SRA FDC_BASE + 0x0 // solo lectura

#define FDC_SRB FDC_BASE + 0x1 // solo lectura
#define FDC_DOR FDC_BASE + 0x2 // lectura/escritura
#define FDC_TDR FDC_BASE + 0x3 // lectura/escritura

#define FDC_MSR FDC_BASE + 0x4 // solo lectura
#define FDC_DSR FDC_BASE + 0x4 // solo escritura

#define FDC_FIFO FDC_BASE + 0x5 // lectura escritura

#define FDC_RESERVADO FDC_BASE + 0x6 // RESERVADO! -> no se usa para nada ;-)

#define FDC_DIR FDC_BASE + 0x7 // solo lectura
#define FDC_CCR FDC_BASE + 0x7 // solo escritura

#define DISPOSITIVO_0 0x00

// posibles estados de retorno de las funciones
#define LECTURA_OK 1
#define ESCRITURA_OK 1

#define LECTURA_ERRONEA -1
#define ESCRITURA_ERRONEA -1

#define OK 1
#define ERROR -1

// tiempo de espera entre reintentos
#define DELAY_FDC 2000L

// tiempo de espera hasta apagar el motor de la disquetera
#define DELAY_APAGA_MOTOR 500000L

// cantidad de reintentos para una operacion con el FDC
#define REINTENTOS 5

// valores para el data rate
#define DATA_RATE_500K 0x00 // disquettes de 3 1/2
#define DATA_RATE_300K 0x01
#define DATA_RATE_250K 0x02 // disquettes de 5 1/4
#define DATA_RATE_1MBPS 0x03

#define FDC_UNIDAD 0x00

// fabricantes de FDC
#define FDC_NEC 0x80
#define FDC_82077 0x90

// comandos del DOR ( Digital Output Register )
#define CMD_ENCENDER_MOTOR 0x10
#define CMD_ACTIVA_RESET 0x00
#define CMD_APAGAR_MOTOR 0x00
#define CMD_NO_RESET 0x04
#define DMA 0x08

// comando para consultar el estado de la IRQ
#define CMD_ESTADO_IRQ 0x08

// comando especify
#define CMD_ESPECIFY 0x03

// comando recalibrar
#define CMD_RECALIBRAR 0x07

// comando para obtener la version del FDC
#define CMD_VERSION 0x10

// comando lectura
#define CMD_LECTURA 0x06 // Lectura
#define FLAG_MT 0x80 // Multi Track
#define FLAG_MFM 0x40 // Modified Frecuency Modulation -> si va en 1 DD
#define FLAG_SC 0x20 // Skip Control
#define FLAG_BPS_512 0x02 // si va 2, entonces 512 bytes por sector
#define FDC_GAP       0x1B

// comando SEEK
#define CMD_SEEK 0x0F

/**
 * Funcion que representa a la tarea de la IRQ 6
 */
void disquetera_handler (dword irq);

/**
 * escribir_byte: envia un byte al registro FIFO del FDC
 * Funcion echa en base al pseudo-codigo del manual del 82077AA de Intel
 *
 * @param la palabra que se va a escribir
 * @returns estado de la operacion
 */
int escribir_byte(byte);

/**
 * leer_byte: lee un byte del registro FIFO del FDC
 * Funcion echa en base al pseudo-codigo del manual del 82077AA de Intel
 *
 * @returns el byte leido o LECTURA_ERRONEA si algo fallo
 */
byte leer_byte();

/**
 * inicializar_fdc: resetea el controlador de disquete y lo inicializa
 *                  con los valores necesarios.
 * Nota: por el momento se va a setear al FDC en modo NO-DMA
 */
int inicializar_fdc ();

/**
 *
 */
int seek (byte, byte);

/**
 * enciende el motor de la disquetera
 */
void encender_motor ();

/**
 * Apaga el motor de la disquetera
 */
void apagar_motor ();

/**
 * Lee un sector de disco
 */
int leer_sector (dword,dword,dword,void*);

/**
 * Periodicamente apagara el motor de la disquetera
 */
void tarea_apagar_motor ();

/**
 * Metodo analogo al memcpy, en realidad deberia ir en otro lado...
 * (usado para realizar el movimiento del buffer de lectura de la disquetera
 * a un buffer interno del metodo que lee).
 */
void* copiar_memoria (void* origen, void* destino, unsigned int tamanio);


#endif //_DISKETE_H_
