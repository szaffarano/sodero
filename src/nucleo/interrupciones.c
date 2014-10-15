#include <sodero/idt.h>
#include <sodero/init.h>
#include <sodero/tss.h>
#include <sodero/teclado.h>
#include <video.h>
#include <sodero/interrupciones.h>
#include <sodero/puertos.h>
#include <sodero/admin_procesos.h>
#include <sodero/syscalls.h>

void setear_gate_int ( int numero, dword tarea );

/**
 * Utilizada por el handler de la interrupcion 13 para informar en que
 * posicion de memoria se estaba alocando.
 */
extern dword ultima_posicion;

void setear_interrupcion ( int, handler_interrupcion );
void setear_irq ( int, handler_irq );

/**
 * Contador para comparar con el QUANTUM
 */
int contador;

/**
 * PCB que representa el scheduler (necesitamos tenerla declarada para
 * cuando hagamos el context switch desde el timer, IRQ0)
 */
extern pcb* pcb_scheduler;

/**
 * Puntero a la GDT
 */
extern gdt* _gdt;

/**
 * Puntero a la tarea que esta ejecutando actualmente.
 */
extern pcb* tarea_actual;

/**
 * el estado de la disquetera (FDC)
 */
extern dword fd_done;

/**
 * Puntero a la IDT (Interrupt Descriptor Table) declarado en main.c
 */
extern idt* _idt;

/**
 * Modo de apropiacion  (0: no apropiativo - 1: apropiativo)
 */
extern int apropiativo;

/**
 * Funcion generica para manejar las interrupciones, implementada en
 * aquellas interrupciones donde no hay un handler especifico para
 * manejarlas.
 */
void handler (void) {
   imprimir ("Interrupcion por default\n");
   EXIT_INT
}

/**
 * Handler para la IRQ numero 1
 * @param numero el numero de la interrupcion que se disparo
 */
void irq1_handler (int numero) {
   imprimir ("se disparo la IRQ1\n");
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 2
 * @param numero el numero de la interrupcion que se disparo
 */
void irq2_handler (int numero) {
   imprimir ("se disparo la IRQ2\n");
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 3
 * @param numero el numero de la interrupcion que se disparo
 */
void irq3_handler (int numero) {
   imprimir ("se disparo la IRQ3\n");
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 4
 * @param numero el numero de la interrupcion que se disparo
 */
void irq4_handler (int numero) {
   imprimir ("se disparo la IRQ4\n");
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 5
 * @param numero el numero de la interrupcion que se disparo
 */
void irq5_handler (int numero) {
   imprimir ("se disparo la IRQ5\n");
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 7
 * @param numero el numero de la interrupcion que se disparo
 */
void irq7_handler (int numero) {
   imprimir ("se disparo la IRQ7\n");
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 8
 * @param numero el numero de la interrupcion que se disparo
 */
void irq8_handler (int numero) {
   imprimir ("se disparo la IRQ8\n");
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 9
 * @param numero el numero de la interrupcion que se disparo
 */
void irq9_handler (int numero) {
   imprimir ("se disparo la IRQ9\n");
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 10
 * @param numero el numero de la interrupcion que se disparo
 */
void irq10_handler (int numero) {
   imprimir ("se disparo la IRQ10\n");
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 11
 * @param numero el numero de la interrupcion que se disparo
 */
void irq11_handler (int numero) {
   imprimir ("se disparo la IRQ11\n");
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 12
 * @param numero el numero de la interrupcion que se disparo
 */
void irq12_handler (int numero) {
   imprimir ("se disparo la IRQ12\n");
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}

/**
 * Handler para la IRQ numero 13
 * @param numero el numero de la interrupcion que se disparo
 */
void irq13_handler (int numero) {
   imprimir ("se disparo la IRQ13\n");
   outb (0x20, 0xA0);
   outb (0x20, 0x20);
}


/**
 * Handler para la excepcion numero 0
 */
void div_cero() {
   imprimir ("Int 0: Division por cero!\n");
   EXIT_INT
}

/**
 * Handler para la excepcion numero 1
 */
void debug_exception() {
   imprimir ("Int 1: debug exception!\n");
  while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 2
 */
void nm2_interrupt() {
   imprimir ("Int 2: NM II interrupt!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 3
 */
void break_exception() {
   imprimir ("Int 3: break exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 4
 */
void overflow_exception() {
   imprimir ("Int 4: overflow exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 5
 */
void bound_exception() {
   imprimir ("Int 5: bound exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 6
 */
void opcode_exception() {
   imprimir ("Int 6: opcode exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 7
 */
void no_math_exception() {
   imprimir ("Int 7: No Math exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 8
 */
void dword_fault() {
   imprimir ("Int 8: doble fault!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 9
 */
void co_cpu_overrun() {
   imprimir ("Int 9: Co CPU overrun!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 10
 */
void tss_exception() {
   imprimir ("Int 10: TSS exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 11
 */
void present_exception() {
   imprimir ("Int 11: present exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 12
 */
void stack_exception() {
   imprimir ("Int 12: stack exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 13
 */
void protection_exception(const dword cod) {
   imprimir ( "Ultima posicion: 0x%x\n", ultima_posicion );
   imprimir ("Int 13: protection exception(0x%x)!\n", (unsigned int)cod);
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 14
 */
void pf_exception() {
   imprimir ("Int 14: page fault exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 15
 */
void reserved_exception() {
   imprimir ("Int 15: reserved exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 16
 */
void math_fault() {
   imprimir ("Int 16: math fault!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 17
 */
void align_exception() {
   imprimir ("Int 17: align exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Handler para la excepcion numero 18
 */
void machine_exception() {
   imprimir ("Int 18: machine exception!\n");
   while (1);
   EXIT_INT
}

/**
 * Funcion que setea la tabla de interrupciones con las funciones declaradas
 * previamente.
 */
void setear_interrupciones() {
   static int i = 0;
   contador = 0;

   /**
    * Se setean las 18 excepciones del I32 en modo protegido
    */
   setear_interrupcion ( 0, div_cero );
   setear_interrupcion ( 1, debug_exception );
   setear_interrupcion ( 2, nm2_interrupt );
   setear_interrupcion ( 3, break_exception );
   setear_interrupcion ( 4, overflow_exception );
   setear_interrupcion ( 5, bound_exception );
   setear_interrupcion ( 6, opcode_exception );
   setear_interrupcion ( 7, no_math_exception );
   setear_interrupcion ( 8, dword_fault );
   setear_interrupcion ( 9, co_cpu_overrun );
   setear_interrupcion ( 10, tss_exception );
   setear_interrupcion ( 11, present_exception );
   setear_interrupcion ( 12, stack_exception );
   setear_interrupcion ( 13, (handler_interrupcion)protection_exception );
   setear_interrupcion ( 14, pf_exception );
   setear_interrupcion ( 15, reserved_exception );
   setear_interrupcion ( 16, math_fault );
   setear_interrupcion ( 17, align_exception );
   setear_interrupcion ( 18, machine_exception );
   //setear_interrupcion ( 0x40, (handler_interrupcion)sys_call_handler );

   /**
    * Se setean las IRQs con sus handlers correspondientes
    * Nota: cada handler de una IRQ sera una tarea independiente!
    */
   setear_irq ( 0, irq0_handler_asm );
   setear_irq ( 1, irq1_handler_asm );
   setear_irq ( 2, irq2_handler_asm );
   setear_irq ( 3, irq3_handler_asm );
   setear_irq ( 4, irq4_handler_asm );
   setear_irq ( 5, irq5_handler_asm );
   setear_irq ( 6, irq6_handler_asm );
   setear_irq ( 7, irq7_handler_asm );
   setear_irq ( 8, irq8_handler_asm );
   setear_irq ( 9, irq9_handler_asm );
   setear_irq ( 10, irq10_handler_asm );
   setear_irq ( 11, irq11_handler_asm );
   setear_irq ( 12, irq12_handler_asm );
   setear_irq ( 13, irq13_handler_asm );
   setear_irq ( 14, irq14_handler_asm );
   setear_irq ( 15, irq15_handler_asm );

   // handler para las system calls!!
   //setear_irq ( 0x20, (handler_irq)sys_call_handler_asm );
   //set_system_gate(0x40,sys_call_handler_asm);
   setear_gate_int ( 0x20, (dword)sys_call_handler );

   /**
    * Las interrupciones desde la ultima IRQ mas uno hasta el limite
    * de interrupciones se setean con el default handler.
    */
   for ( i = (0x20 + 16); i < 0x40; i++ ) {
      setear_interrupcion ( i, handler );
   }

   for ( i = (0x40 + 1); i < CANT_DESCRIPT; i++ ) {
      setear_interrupcion ( i, handler );
   }
}

/**
 * Funcion que setea una interrupcion para que sea ejecutada como un
 * TASK GATE.
 *
 * @param numero numero de IRQ a setear (relativa a 0x20)
 * @param tarea puntero a la funcion que representa la tarea de atencion
 *        de la IRQ.
 */
void setear_irq ( int numero, handler_irq tarea ) {
   tss* tss_int;
   int idx = siguiente_indice_gdt();

   tss_int = (tss*) sys_alocar ( sizeof (tss) );
   setear_desc_tss ( tss_int, &_gdt->descriptores [idx] );
   setear_tarea ( tss_int, tarea );
   tss_int->eflags = 0x2; // deshabilitamos las interrupciones

   _idt->descriptores[0x20 + numero].desc_gate.selector_tss = idx * 8;
   _idt->descriptores[0x20 + numero].desc_gate.reservado0 = 0x0;
   _idt->descriptores[0x20 + numero].desc_gate.reservado1 = 0x0;
   _idt->descriptores[0x20 + numero].desc_gate.tipo = 0x5;
   _idt->descriptores[0x20 + numero].desc_gate.cero = 0x0;
   _idt->descriptores[0x20 + numero].desc_gate.dpl = 0x0;
   _idt->descriptores[0x20 + numero].desc_gate.presente = 0x1;
   _idt->descriptores[0x20 + numero].desc_gate.reservado2 = 0x0;
}

/**
 * Funcion que setea una interrupcion para que sea ejecutada como un
 * TRAP GATE.
 *
 * @param numero numero de IRQ a setear (relativa a 0x20)
 * @param tarea puntero a la funcion que representa el trap de atencion
 *        de la interrupcion.
 */
void setear_gate_int ( int numero, dword tarea ) {
   word offset;

   offset = (tarea & 0x0000FFFF); 
   _idt->descriptores[0x20 + numero].desc_int_gate.offset_0_15 = offset;
   _idt->descriptores[0x20 + numero].desc_int_gate.selector_segmento = 0x08;
   _idt->descriptores[0x20 + numero].desc_int_gate.reservado1 = 0x0;
   _idt->descriptores[0x20 + numero].desc_int_gate.tipo = 15;
   _idt->descriptores[0x20 + numero].desc_int_gate.cero = 0x0;
   _idt->descriptores[0x20 + numero].desc_int_gate.dpl = 0x0;
   _idt->descriptores[0x20 + numero].desc_int_gate.presente = 0x1;

   offset = ( (tarea >> 16) & 0x0000FFFF ); 
   _idt->descriptores[0x20 + numero].desc_int_gate.offset_16_31 = offset;

}

/**
 * Funcion que setea una interrupcion para que sea ejecutada como un
 * TASK GATE.  Es complementaria a setear_interrupt ().
 *
 * @param numero numero de IRQ a setear (relativa a 0x20)
 * @param interrupt puntero a la funcion que representa la tarea de atencion
 *        de la IRQ.
 */
void setear_interrupcion ( int numero, handler_interrupcion interrupt ) {
   _idt->descriptores[numero].desc_int.handler = (dword)interrupt;
   _idt->descriptores[numero].desc_int.selector = 0x08;
   _idt->descriptores[numero].desc_int.reservado = 0x0;
   _idt->descriptores[numero].desc_int.gpl = 0x8E;
   _idt->descriptores[numero].desc_int.ultimo = 0x0;
}
