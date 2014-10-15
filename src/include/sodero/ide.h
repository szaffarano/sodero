#ifndef _IDE_H_
#define _IDE_H_

#include <sodero/registros.h>
#include <sodero/definiciones.h>
#include <sodero/puertos.h>

#define BASE_PRIMARIO 0x1F0
#define BASE_SECUNDARIO 0x170

#define ID_MASTER 0xA0
#define ID_ESCLAVO 0xB0

#define PRESENTE 1
#define AUSENTE 0

#define DESCONOCIDO 0
#define CDROM 1
#define HD 2

#define MASCARA_IRQ14 0x4000
#define MASCARA_IRQ15 0x8000

// registros
#define REG_DATOS 0
#define REG_FEATURE 1
#define REG_CONTADOR 2
#define REG_MOTIVO 2
#define REG_SECTOR 3
#define REG_LSB_CYL 4
#define REG_MSB_CYL 5
#define REG_SEL_DRIVER 6
#define REG_ESTADO 7
#define REG_COMANDO 7
#define REG_CONTROL 0x206

#define COMANDO_ID_HD 0xEC
#define COMANDO_ID_CDROM 0xA1
#define COMANDO_PAQUETE 0xA0

#define FASE_SIN_COMANDO 9
#define FASE_FINALIZO 3

typedef struct {
   word puerto_base; // IDE0/IDE1
   byte id_dispositivo; // MASTER/ESCLAVO
   byte tipo;
   byte presente;
} descriptor_ide;

typedef struct {
    word flags;
    word logical_cylinders;
    word reserved;
    word logical_heads;
    word misc1;
    word misc2;
    word logical_sectors;
    word misc3;
    word misc4;
    word misc5;
    byte serial[20];
    word misc6;
    word misc7;
    word vend_bytes;
    byte firmware[8];
    byte model[40];
    word max_sectors_per_interrupt;
    word misc8;
    word capabilities1;
    word capabilities2;
    word pio;
    word dma;
    word misc10;
    word current_cylinders;
    word current_heads;
    word current_sectors;
    word current_size_in_sectors_lo;
    word current_size_in_sectors_hi;
    word current_max_sectors_per_interrupt;
    word current_addresseable_sectors_lo;
    word current_addresseable_sectors_hi;
    word misc11;
    word dma_mode;
    word advanced_pio_mode;
    word min_dma_cycles;
    word recommended_dma_cycles;
    word min_noflow_pio_cycles;
    word min_iordy_pio_cycles;
    word misc12;
    word misc13;
    word misc14;
    word misc15;
    word misc16;
    word misc17;
    word queue_depth;
    word misc18;
    word misc19;
    word misc20;
    word misc21;
    word major_version;
    word minor_version;
    word supported_features_1;
    word supported_features_2;
    word supported_features_3;
    word enabled_features_1;
    word enabled_features_2;
    word enabled_features_3;
    word dma_mode_2;
    word security_erase_time;
    word enhanced_security_earse_time;
    word current_advance_power_value;
    word master_password_revision_code;
    word hardware_reset_result;
    byte misc22[ 0xfe - 0xbc ];
    word removeable_media_status_features;
    word security_status;
    byte misc23[ 0x140 - 0x102 ];
    word cfa_power_mode_1;
    byte mode24[ 0x150 - 0x142 ];
    byte mode25[ 0x1fe - 0x150 ];
    word integrity;
} ide_id;

int detectar_ide (const word);
int detectar_dispositivo (const word, const byte);
int leer_estado ( const word, const word, const word, dword);
int identificar_dispositivo (const word);
int esperar_irq (const int, unsigned int);
void leer_puerto (const word, word*, unsigned int);
void escribir_puerto (const word, word*, unsigned int);
word* obtener_cadena (word*, unsigned int, unsigned int);
int seleccionar_dispositivo (word, byte);
void inicializar_ide();
int habilitar_dispositivo ( const word );

#endif // _IDE_H_
