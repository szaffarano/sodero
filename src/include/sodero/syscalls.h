#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_

#include <sodero/definiciones.h>
#include <video.h>
#include <sodero/memoria.h>

/** 
 * Tipo de dato representativo de un puntero a funcion
 */
typedef int (*puntero_funcion)();

#define	CANT_SYS_CALLS		20

#define	SYS_PRINTC		0 // OK
#define	SYS_GETC		1 // OK
#define	SYS_ABRIR		2
#define	SYS_CERRAR		3
#define	SYS_LEER		4
#define	SYS_ESCRIBIR		5
#define	SYS_ALOCAR		6 // OK
#define	SYS_EJECUTAR		7 // OK
#define	SYS_SALIR		8 // OK
#define	SYS_PAUSA		9 // OK
#define	SYS_MATAR		10
#define	SYS_CD			11
#define	SYS_ESPERAR_PID		12 // OK
#define	SYS_ABRIR_DIRECTORIO	13
#define	SYS_LEER_ENTRADA_DIR	14
#define	SYS_DIR                 15
#define	SYS_PS                  16 //OK
#define	SYS_REBOOT              17 //OK
#define	SYS_XCHG_APROP          18 //OK
#define	SYS_LISTAR_DIRECTORIO   19 //OK


extern int sys_printc ();
extern int sys_getc ();
extern int sys_abrir ();
extern int sys_cerrar ();
extern int sys_leer ();
extern int sys_escribir ();
extern int sys_alocar ();
extern int sys_ejecutar ();
extern int sys_salir ();
extern int sys_pausa ();
extern int sys_matar ();
extern int sys_cd ();
extern int sys_esperar_pid ();
extern int sys_abrir_dir ();
extern int sys_leer_entrada_dir ();
extern int sys_dir ();
extern int sys_ps ();
extern int sys_reboot ();
extern int sys_xchg_aprop ();
extern int sys_listar_directorio ();

/**
 * llamar_sys_call: funcion que centraliza la llamada a una system call
 * su implementacion esta realizada en assembler (@see usuario.asm)
 *
 * @param (dword) numero de system call a llamar
 *
 * @param (dword) primer argumento
 *
 * @param (dword) segundo argumento
 *
 * @param (dword) tercer argumento
 *
 * @return (int) exito de la operacion
 */
int llamar_sys_call ( dword, dword, dword, dword );

#endif // _SYS_CALLS_H_
