#include <sodero/syscalls.h>
#include <sodero/teclado.h>


puntero_funcion tabla_sys_calls[] = { sys_printc, sys_getc,     sys_abrir,
                                      sys_cerrar, sys_leer,     sys_escribir,
                                      sys_alocar, sys_ejecutar, sys_salir,
                                      sys_pausa,  sys_matar,    sys_cd,
                                      sys_esperar_pid, 
                                      sys_abrir_dir,
                                      sys_leer_entrada_dir, 
                                      sys_dir, sys_ps, sys_reboot, 
                                      sys_xchg_aprop, sys_listar_directorio };

int sys_cerrar () {
   //imprimir ( "sys call cerrar!\n" );
   return 0;
}

int sys_escribir () {
   imprimir ( "sys call escribir!\n" );
   return 0;
}

int sys_cd () {
   imprimir ( "sys call cd!\n" );
   return 0;
}
