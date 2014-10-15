#include <sodero/sys_video.h>
#include <video.h>
#include <sodero/registros.h>
#include <sodero/interrupciones.h>
#include <sodero/idt.h>
#include <sodero/tss.h>
#include <sodero/gdt.h>
#include <sodero/segmento.h>
#include <sodero/init.h>
#include <sodero/memoria.h>
#include <sodero/syscalls.h>
#include <sodero/proceso.h>
#include <sodero/puertos.h>
#include <sodero/definiciones.h>
#include <timer.h>
#include <sodero/dma.h>
#include <sodero/disquete.h>
#include <sodero/tareas.h>
#include <sodero/admin_procesos.h>
#include <sodero/temporizador.h>
#include <sodero/teclado.h>
#include <speaker.h>
#include <sodero/ide.h>
#include <fat.h>


void tarea_dummy1 ();
void tarea_dummy2 ();

unsigned int memorySize;
unsigned int _lowMemorySize;
unsigned int _biosMemorySize;
unsigned int _bootDrive;
unsigned int _bssSize;
unsigned int _kernelSize;


/**
 * Puntero a la GDT (Global Descriptor Table)
 */
gdt* _gdt;

/**
 * Puntero a la IDT (Interrupt Descriptor Table)
 */
idt* _idt;

/**
 * Indice de alocacion de memoria
 */
extern dword ultima_posicion;

/**
 * Puntero al inicion de la lista de procesos
 */
extern nodo* lista_ready_inicio;

/**
 * Puntero al final de la lista de procesos
 */
extern nodo* lista_ready_fin;

/**
 * PCB que representa el scheduler (necesitamos tenerla declarada para
 * cuando hagamos el context switch desde el timer, IRQ0)
 */
extern pcb* pcb_scheduler;


int inicio_memoria;

int main () {
   pcb* aux = NULL;

   //char mensaje_bienvenida[26] = "Bienvenidos al banquete!\0";
   //char subrayado[26] = "~~~~~~~~~~~ ~~ ~~~~~~~~\n\0";

   //int len_msg = longitud_cadena (mensaje_bienvenida);

   lista_ready_inicio = NULL;
   lista_ready_fin = NULL;
   ultima_posicion = _lowMemorySize + 2 * 1024 * 1024;
   //ultima_posicion = 1024 * 1024;
   //ultima_posicion = inicio_memoria;

   setear_siguiente_indice (3);

   setear_interrupciones ();

   // Seteo IDT
   cargar_idt ( _idt, CANT_DESCRIPT * 8 - 1 );

   habilitar_irqs ();

   inicializa_teclado();

   /**
    * Se crea la tarea init y se agrega a la cola de ready
    * Tambien se setea el TR con el selector de su TSS, con lo
    * que al hacer el primer context switch (ir_a_tarea()) quedara
    * asignada init como la "tarea de sistema".
    * nota: por ahora init va a ser la tarea idle
    */
   aux = crear_tarea (init);
   aux->dato = (char*)alocar (5);
   aux->dato = "INIT";
   aux->tipo = PROCESO_SISTEMA;
   agregar (aux);
 
   cargar_tr (aux->selector * 8);

   setear_temporizador ();

   pcb_scheduler = crear_tarea (admin_procesos);
   pcb_scheduler->dato = (char*)alocar (6);
   pcb_scheduler->dato = "ADMIN\0";
   pcb_scheduler->tipo = PROCESO_SISTEMA;
   pcb_scheduler->tarea->eflags = 0x2;

   sti();

   //inicializar_ide();

   inicializar_fdc();

   imprimir ( "Bienvenidos al banquete!\n\n" );

   sys_ejecutar ( "INTERFAZ", NULL );

   while (1);

   return 0;
}

void tarea_dummy1 () {
   int contador;
   contador = 100;
   //llamar_sys_call ( SYS_ABRIR, (dword)"<nombre_archivo>", 1, 1 );
   while (1) {
//      setear_coordenadas ( 10, 18);
//      imprimir ("contador: %d\n", contador++);
//      delay ( MICRO(1000000L) );
   }
}

void tarea_dummy2 () {
   llamar_sys_call ( SYS_LEER, 199, 1, 1 );
   //test_fat ();
   while (1) {
   }
}
