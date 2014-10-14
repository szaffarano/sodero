#include <sodero/syscalls.h>
#include <matar.h>

int abrir ( char* nombre ) {
   return llamar_sys_call ( SYS_ABRIR, (dword)nombre, 0, 0 );
}

int leer ( int fd, void* buff, unsigned int tam ) {
   return llamar_sys_call ( SYS_LEER, fd, (dword)buff, tam );
}
