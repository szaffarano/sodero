#include <sodero/syscalls.h>
#include <dir.h>

int abrir_directorio (dir* directorio) {
   return llamar_sys_call ( SYS_ABRIR_DIRECTORIO, (dword)directorio, 0, 0 );
}

int leer_entrada_dir (char* str, dir* directorio) {
   return llamar_sys_call ( SYS_LEER_ENTRADA_DIR,
                            (dword)str,
                            (dword)directorio,
                            0 );
}

int listar_directorio () {
   return llamar_sys_call ( SYS_LISTAR_DIRECTORIO, 0, 0, 0 );
}
