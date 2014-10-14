
#ifndef _ES_H_
#define _ES_H_

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
int abrir ( char* );

/**
 * sys_abrir: syscall encargada de leer sobre un archivo
 */
int leer ( int, void*, unsigned int);

#endif // _ES_H_
