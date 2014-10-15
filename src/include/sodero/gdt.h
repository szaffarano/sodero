#ifndef _GDT_H_
#define _GDT_H_

/**
 * sodero/gdt.h
 * Esta biblioteca tiene las deficiones de los tipos de datos necesarios
 * para manipular la GDT (Global Descriptor Table, Tabla Global de Descriptores)
 * desde C en forma analoga a assembler.
 */

#include <sodero/definiciones.h>
#include <sodero/tss.h>
#include <sodero/segmento.h>


/**
 * Esta union representa un descriptor (entrada) dentro de la GDT, debido a
 * que la misma puede contener diferentes tipos de descriptores, sera
 * necesario agregar a esta union todos dichos posibles tipos, entonces
 * cuando se referencie una variable de tipo "descriptor" habra que
 * tener en cuenta a que tipo de descriptor se esta referenciando, para
 * ello hay que explicitarlo:
 * 
 * descriptor desc;
 * desc.desc_tss // referenciara a un descriptor de un TSS
 * desc.desc_seg // referenciara a un descriptor de un segmento
 */
typedef union {
   tss_desc desc_tss;
   seg_desc desc_seg;
} descriptor;

/**
 * Estructura que representa a la GDT propiamente dicha, la misma
 * contiene un arreglo de 2^13 = 8192 posibles descriptores que puede
 * tener seteada la GDT.  La direccion a donde apunta esta estructura
 * se setea en el sodero.asm, ya que la misma debe estar a continuacion
 * del segmento BSS y alineada a 4k.
 */
typedef struct {
   descriptor descriptores[8192];
} gdt;

/**
 * Enum para identificar a un descriptor de segmento como de codigo, de datos,
 * etc.
 */
typedef enum {
     SEG_SISTEMA,
     SEG_CODIGO,
     SEG_DATOS,
     SEG_PILA
} tipo_segmento;

/**
 * Esta funcion setea los valores (base, limite y bits de segmento) de
 * un descriptor de tss, en base a la TSS que se le pasa como primer
 * argumento.
 */
void setear_desc_tss (const tss*, descriptor*);

/**
 * Funcion que realiza un dump de un descriptor de un TSS para conocer 
 * sus valores.
 */
void ver_descriptor (const descriptor*);

/**
 * Devuelve el siguiente indice a asignar en la GDT
 */
word siguiente_indice_gdt ();

/**
 * Setea el siguiente indice en la GDT (cuando implementemos un algoritmo
 * mas controlado, esta funcion no se va a usar mas)
 */
void setear_siguiente_indice (const word sig);

/**
 * Funcion que setea un descriptor de segmento de codigo o de datos 
 * asignandole el tipo, la base y el limite
 * Mas adelante se agregara un quinto argumento, para parametrizar mejor las 
 * propiedades del seg
 */
void setear_desc_seg (const tipo_segmento tipo, 
                      const dword base, 
                      descriptor* desc, 
                      const dword tam);

/**
 * Funcion que realiza un dump de un descriptor de Segmento para conocer 
 * sus valores a partir del Indice dentro de la GDT donde se encuentre este
 * descriptor.
 */
void ver_desc_seg (const int);
/**
 * Define del tamanio de la PILA que poseen los procesos de USUARIO = 4Kb
 */
#define TAMANIO_PILA_USUARIO 1024 * 4 // 4 k = una pagina...

#endif // _GDT_H_
