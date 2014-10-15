#ifndef _SYS_ES_H_
#define _SYS_ES_H_

/**
 * sys_es.h Biblioteca que incluye los include necesarios para el 
 * funcionamiento de las sys_call relacionadas con entrada/salida.
 */

#include <timer.h>
#include <sodero/registros.h>
#include <sodero/memoria.h>
#include <video.h>
#include <sodero/idt.h>
#include <fat.h>
#include <sodero/disquete.h>
#include <sodero/proceso.h>
#include <sodero/gdt.h>

/**
 * sys_abrir: syscall encargada de abrir un archivo
 */
int sys_abrir ( char* );

/**
 * sys_abrir: syscall encargada de leer sobre un archivo
 */
int sys_leer ( int, void*, unsigned int);

#endif // _SYS_ES_H_
