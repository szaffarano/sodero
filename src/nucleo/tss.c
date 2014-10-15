#include <sodero/tss.h>
#include <video.h>
#include <sodero/definiciones.h>
#include <sodero/memoria.h>
#include <sodero/gdt.h>
#include <sodero/syscalls.h>

/**
 * Pone en cero a toda la TSS que recibe como parametro
 *
 * @param tss_tarea TSS que se va a inicializar a cero
 */
void inicializar_tss (tss* tss_tarea) {
   setear_memoria (tss_tarea, 0, sizeof (tss));
}

/**
 * Funcion que realiza un dump de la TSS que recibe como parametro
 *
 * @param tss_usuario TSS a mostrar
 */
void ver_tss (const tss* tss_usuario) {
   imprimir ("registros SS:0x%xw ES:0x%xw CS:0x%xw DS:0x%xw \
          FS:0x%xw GS:0x%xw ESP:0x%x EIP:0x%x\n", tss_usuario->ss,
                                                  tss_usuario->es,
                                                  tss_usuario->cs,
                                                  tss_usuario->ds,
                                                  tss_usuario->fs,
                                                  tss_usuario->gs,
                                                  tss_usuario->esp,
                                                  tss_usuario->eip);
}

/**
 * Funcion que setea una TSS con los valores necesarios para
 * realizar el context switch (segmentos, eflags, eip, etc).
 *
 * @param tss_usuario TSS que se va a setear
 * @param tarea puntero a una funcion que representa la tarea (proceso) 
 *              que se va a ejecutar por la TSS que se esta seteando
 */
void setear_tarea (tss* tss_usuario, const handler_tarea tarea) {
   tss_usuario->eip = (dword)tarea;

   tss_usuario->ss = get_ss ();
   tss_usuario->ss0 = get_ss ();
   tss_usuario->ss1 = get_ss ();
   tss_usuario->ss2 = get_ss ();

   //tss_usuario->esp = get_esp ();

   //Se aloca en crear_tarea
   tss_usuario->esp = (dword)sys_alocar ( (dword)TAMANIO_PILA_USUARIO ) + 
                      (dword)TAMANIO_PILA_USUARIO;//sumo el tamanio de la pila
                                              //para apuntar el esp al final
   tss_usuario->esp0 = tss_usuario->esp;
   tss_usuario->esp1 = tss_usuario->esp;
   tss_usuario->esp2 = tss_usuario->esp;

   tss_usuario->es = get_es ();
   tss_usuario->cs = get_cs ();
   tss_usuario->ds = get_ds ();
   tss_usuario->fs = get_fs ();
   tss_usuario->gs = get_gs ();

/**************
Seteo por Default del Seteo de los FLAGS o Banderas del sistema 

Bits del Registro EFLAGS
31   27   23   19   15   11   7    3
0000 0000 0000 0000 0000 0010 0000 0010
                           I         1
I: Interrupciones (Habilito las interrupciones)
1: reservado (reservado Intel, siempre en 1)

***************/

   tss_usuario->eflags = 0x202;
}
