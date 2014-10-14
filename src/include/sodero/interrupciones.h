#ifndef _INTERRUPCIONES_H_
#define _INTERRUPCIONES_H_

/**
 * sodero/interrupciones.h
 * biblioteca que contiene los encabezados de todas las funciones
 * asociadas a las interrupciones del sistema.
 */

/**
 * Representa la funcion asociada a la tarea que atiende a un IRQ
 */
typedef void (*handler_irq)(void);

/**
 * Representa la funcion asociada a la tarea que atiende a una interrupcion
 */
typedef void (*handler_interrupcion)(void);

/**
 * Handler de interrupcion generico
 */
void handler(void);

/**
 * Handler para la interrupcion numero 0
 */
void div_cero();

/**
 * Handler para la interrupcion numero 1
 */
void debug_exception();

/**
 * Handler para la interrupcion numero 2
 */
void nm2_interrupt();

/**
 * Handler para la interrupcion numero 3
 */
void break_exception();

/**
 * Handler para la interrupcion numero 4
 */
void overflow_exception();

/**
 * Handler para la interrupcion numero 5
 */
void bound_exception();

/**
 * Handler para la interrupcion numero 6
 */
void opcode_exception();

/**
 * Handler para la interrupcion numero 7
 */
void no_math_exception();

/**
 * Handler para la interrupcion numero 8
 */
void dword_fault();

/**
 * Handler para la interrupcion numero 9
 */
void co_cpu_overrun();

/**
 * Handler para la interrupcion numero 10
 */
void tss_exception();

/**
 * Handler para la interrupcion numero 11
 */
void present_exception();

/**
 * Handler para la interrupcion numero 12
 */
void stack_exception();

/**
 * Handler para la interrupcion numero 13
 */
void protection_exception(const dword);

/**
 * Handler para la interrupcion numero 14
 */
void pf_exception();

/**
 * Handler para la interrupcion numero 15
 */
void reserved_exception();

/**
 * Handler para la interrupcion numero 16
 */
void math_fault();

/**
 * Handler para la interrupcion numero 17
 */
void align_exception();

/**
 * Handler para la interrupcion numero 18
 */
void machine_exception();


/**
 * Funcion que realiza el seteo de los handlers de interrupciones dentro
 * de la IDT
 */
void setear_interrupciones ();

/**
 * Funcion que atiende a la IRQ 1
 */
void irq1_handler(int);

/**
 * Funcion que atiende a la IRQ 2
 */
void irq2_handler(int);

/**
 * Funcion que atiende a la IRQ 3
 */
void irq3_handler(int);

/**
 * Funcion que atiende a la IRQ 4
 */
void irq4_handler(int);

/**
 * Funcion que atiende a la IRQ 5
 */
void irq5_handler(int);

/**
 * Funcion que atiende a la IRQ 6
 */
void irq6_handler(int);

/**
 * Funcion que atiende a la IRQ 7
 */
void irq7_handler(int);

/**
 * Funcion que atiende a la IRQ 8
 */
void irq8_handler(int);

/**
 * Funcion que atiende a la IRQ 9
 */
void irq9_handler(int);

/**
 * Funcion que atiende a la IRQ 10
 */
void irq10_handler(int);

/**
 * Funcion que atiende a la IRQ 11
 */
void irq11_handler(int);

/**
 * Funcion que atiende a la IRQ 12
 */
void irq12_handler(int);

/**
 * Funcion que atiende a la IRQ 13
 */
void irq13_handler(int);

/**
 * Funcion que atiende a la IRQ 14
 */
void irq14_handler(int);

/**
 * Funcion que atiende a la IRQ 15
 */
void irq15_handler(int);

/**
 * Funcion que representa a la tarea de la IRQ 0
 */
void irq0_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 1
 */
void irq1_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 2
 */
void irq2_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 3
 */
void irq3_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 4
 */
void irq4_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 5
 */
void irq5_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 7
 */
void irq6_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 7
 */
void irq7_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 8
 */
void irq8_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 9
 */
void irq9_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 12
 */
void irq10_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 11
 */
void irq11_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 12
 */
void irq12_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 13
 */
void irq13_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 14
 */
void irq14_handler_asm();

/**
 * Funcion que representa a la tarea de la IRQ 15
 */
void irq15_handler_asm();

void sys_call_handler_asm ();
//int sys_call_handler_asm ();

void sys_call_handler ( int par1, int par2, int par3, int par4 );


#endif // _INTERRUPCIONES_H_
