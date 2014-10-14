#ifndef _CDROM_H_
#define _CDROM_H_

#include <sodero/ide.h>
#include <video.h>
#include <sodero/registros.h>
#include <sodero/puertos.h>
#include <sodero/definiciones.h>
#include <timer.h>

#define CDROM_START_STOP 0x1B
#define CDROM_PLAY 0x47
#define CDROM_LISTAR 0x43
#define CDROM_PAUSA 0x4B

extern descriptor_ide descs_ide[4];

// nuevas
int cdrom_cmd ( int, unsigned long, word*, word* );
void ejecutar_cd ( int );

// viejas
int play_cd ( dword base, byte id, byte min_xx, word fr_sec );
int listar_cd ( dword base, byte id );

void atapi ( dword base, byte id );

#endif //_CDROM_H_
