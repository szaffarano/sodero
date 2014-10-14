#ifndef _IDT_H_
#define  _IDT_H_

/**
 * sodero/idt.h
 * biblioteca que contiene todas las deficiones relacionadas con la
 * IDT (Interrupt Descriptor Table, Tabla de Descriptores de Interrupcion)
 * del sistema.
 */

#include <sodero/definiciones.h>

#define CANT_DESCRIPT 256

// macro que sale de una funcion en C como si fuese desde una interrupcion
#define EXIT_INT __asm__ __volatile__ ("mov %ebp, %esp\n"\
                                       "pop %ebp\n"\
                                       "iret\n");

#define EXIT_IRQ __asm__ __volatile__ ("mov %ebp, %esp\n"\
                                       "pop %ebp\n"\
                                       "addl $4,%esp\n"\
                                       "iret\n");
 
#define EXIT_FUNC __asm__ __volatile__ ("ret\n");

#define SALIR __asm__ __volatile__ ("ljmp $0x18, $0");

#define PAUSA __asm__ __volatile__ ("ljmp $0x18, $0");

/**
 * El descriptor de una interrupcion.
 */
typedef struct {
   word handler;
   word selector;
   byte reservado; //0x00
   byte gpl; // 0x8E
   word ultimo; //0x00
} NOALIGN int_desc; // 16 + 16 + 8 + 8 + 16 = 8 bytes

/**
 * El descriptor de una task gate.
 */
typedef struct {
   word reservado0;
   word selector_tss;
   byte reservado1;
   unsigned tipo:4; // siempre va 0x5
   unsigned cero:1; // siempre va cero
   unsigned dpl:2;
   unsigned presente:1;
   word reservado2;
} NOALIGN gate_desc;

/**
 * El descriptor de una int gate.
 */
typedef struct {
   word offset_0_15;
   word selector_segmento;
   byte reservado1;
   unsigned tipo:4; // siempre va 15
   unsigned cero:1; // siempre va cero
   unsigned dpl:2;
   unsigned presente:1;
   word offset_16_31;
} NOALIGN int_gate_desc;

typedef union {
   int_desc desc_int;
   gate_desc desc_gate;
   int_gate_desc desc_int_gate;
} idt_desc;

/**
 * La IDT propiamente dicha, la cual, en forma analoga a la GDT contiene un
 * arreglo con todas las interrupciones, las cuales se setearan en tiempo de
 * ejecucion por el kernel del Sistema Operativo.
 */
typedef struct {
   idt_desc descriptores[CANT_DESCRIPT];
} idt;

/**
 * Funcion que realiza un 'lidt' (Load IDT) desde assembler, si bien el header
 * esta declarado en C, la implementacion de dicha funcion esta echa en
 * assembler.
 */
void cargar_idt (const idt* base, const dword limite);

/**
 * Funcion de test utilizada para invocar a una interrupcion.
 */
void llamar_int (void);

/**
 * Funcion de test que simula un bucle infinito desde assembler.
 */
void bucle_infinito (void);

void habilitar_irqs();

#endif //_IDT_H_
